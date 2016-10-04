#include "mltkLinearRegression.h"
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <Eigen/Dense>

const MTypeId mltkLinearRegression::mltkLinearRegressionID = 0x00011; // 値は適当
const MString mltkLinearRegression::coefficientName[3] = {"coefficient", "cv", "Coefficient"};

mltkLinearRegression::mltkLinearRegression()
    : mltkNode()
{
}

mltkLinearRegression::~mltkLinearRegression()
{
}

MStatus mltkLinearRegression::initLinearRegression()
{    
    MStatus status = mltkNode::initNode();
    if (status != MS::kSuccess)
    {
        return status;
    }

    // 多項式係数アトリビュート
    MFnNumericAttribute coefAttrFn;
    MObject coefAttr = coefAttrFn.create(coefficientName[0], coefficientName[1], MFnNumericData::kDouble, 0.0);
    coefAttrFn.setNiceNameOverride(coefficientName[2]);
    coefAttrFn.setArray(true);
    coefAttrFn.setKeyable(false);
    coefAttrFn.setConnectable(false);
    addAttribute(coefAttr);

    return MS::kSuccess;
}

/**
 * @fn MStatus mltkLinearRegression::learn(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output)
 * @brief 学習メソッド．出力データを入力データに関する多項式で近似する．
 */
MStatus mltkLinearRegression::learn(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output)
{
    MStatus status;
    const int dimInputs = input.cols() + 1; // 定数項を加える
    const int dimOutputs = output.cols();

    // 入力データに定数項を追加
    Eigen::MatrixXd extInput = input;
    extInput.conservativeResize(input.rows(), dimInputs);
    for (int r = 0; r < extInput.rows(); ++r)
    {
        extInput(r, dimInputs - 1) = 1.0;
    }
    // 入力行列 × 係数行列 = 出力行列　の連立方程式の求解
    Eigen::MatrixXd x = extInput.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(output);

    // 多項式係数の設定
    MFnDependencyNode thisNodeFn(thisMObject());
    MPlug cplug = thisNodeFn.findPlug(coefficientName[0]);
    cplug.setNumElements(dimInputs * dimOutputs);
    for (int i = 0; i < dimInputs; ++i)
    {
        for (int o = 0; o < dimOutputs; ++o)
        {
            cplug.elementByLogicalIndex(o * dimInputs + i).setDouble(x(i, o));
        }
    }

    // 出力アトリビュートの設定
    if (!thisNodeFn.hasAttribute(outAttrName[0]))
    {
        MFnNumericAttribute outAttrFn;
        MObject outAttr = outAttrFn.create(outAttrName[0], outAttrName[1], MFnNumericData::kDouble, 0.0);
        outAttrFn.setNiceNameOverride(outAttrName[2]);
        outAttrFn.setWritable(false);
        outAttrFn.setArray(true);
        thisNodeFn.addAttribute(outAttr);
    }
    thisNodeFn.findPlug(outAttrName[0]).setNumElements(dimOutputs);
    thisNodeFn.findPlug(dimOutAttrName[0]).setInt(dimOutputs);

    return MS::kSuccess;
}

/**
 * @fn MStatus mltkLinearRegression::compute(const MPlug& plug, MDataBlock& data)
 * @brief computeオーバーライド
 */
MStatus mltkLinearRegression::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status;
    if (status = mltkNode::compute(plug, data), status != MS::kSuccess)
    {
        return status;
    }

    MFnDependencyNode thisNodeFn(thisMObject());
    const int dimInputs = data.inputValue(thisNodeFn.findPlug(dimInAttrName[0])).asInt();
    const int dimOutputs = data.inputValue(thisNodeFn.findPlug(dimOutAttrName[0])).asInt();
    const int plugId = plug.logicalIndex();
    if (plugId >= dimOutputs)
    {
        return MS::kInvalidParameter;
    }

    // 多項式：入力×係数の総和 の計算
    double sum = 0.0;
    MPlug iplug = thisNodeFn.findPlug(inAttrName[0]);
    MPlug cplug = thisNodeFn.findPlug(coefficientName[0]);
    for (int j = 0; j < dimInputs; ++j)
    {
        MStatus istat, cstat;
        MDataHandle inputHandle = data.inputValue(iplug.elementByLogicalIndex(j), &istat);
        MDataHandle coefHandle = data.inputValue(cplug.elementByLogicalIndex(plugId * (dimInputs + 1) + j), &cstat);
        if (istat == MS::kSuccess && cstat == MS::kSuccess)
        {
            sum += coefHandle.asDouble() * inputHandle.asDouble();
        }
    }
    MDataHandle constHandle = data.inputValue(cplug.elementByLogicalIndex(plugId * (dimInputs + 1) + dimInputs), &status);
    if (status == MS::kSuccess)
    {
        sum += constHandle.asDouble();
    }
    MDataHandle outputHandle = data.outputValue(plug);
    outputHandle.setDouble(sum);
    data.setClean(plug);

    return MS::kSuccess;
}
