#include "mltkNode.h"
#include <vector>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MAnimControl.h>
#include <Eigen/Core>

const MTypeId mltkNode::mltkNodeID = 0x00010; // 値は適当に設定
const MString mltkNode::inAttrName[3] = {"input", "iv", "Input"};
const MString mltkNode::outAttrName[3] = {"output", "ov", "Output"};
const MString mltkNode::targetAttrName[3] = {"target", "tv", "Target"};
const MString mltkNode::dimInAttrName[3] = {"dimInputs", "di", "Dim inputs"};
const MString mltkNode::dimOutAttrName[3] = {"dimOutputs", "do", "Dim outputs"};
const MString mltkNode::dimTargetsAttrName[3] = {"dimTargets", "dt", "Dim targets"};

mltkNode::mltkNode()
{
}

mltkNode::~mltkNode()
{
}

MStatus mltkNode::initNode()
{
	// 入力次元数
    MFnNumericAttribute diAttrFn;
    MObject diAttr = diAttrFn.create(dimInAttrName[0], dimInAttrName[1], MFnNumericData::kInt, 0);
    diAttrFn.setNiceNameOverride(dimInAttrName[2]);
    //diAttrFn.setWritable(false); // ユーザー編集は許可したくないが，ファイル保存されなくなるためコメントアウト
    diAttrFn.setStorable(true);
    diAttrFn.setConnectable(false);
    addAttribute(diAttr);

    // 出力次元数
    MFnNumericAttribute doAttrFn;
    MObject doAttr = doAttrFn.create(dimOutAttrName[0], dimOutAttrName[1], MFnNumericData::kInt, 0);
    doAttrFn.setNiceNameOverride(dimOutAttrName[2]);
    //doAttrFn.setWritable(false); // 同上
    doAttrFn.setConnectable(false);
    addAttribute(doAttr);

    // 目標出力次元数
    MFnNumericAttribute dtAttrFn;
    MObject dtAttr = dtAttrFn.create(dimTargetsAttrName[0], dimTargetsAttrName[1], MFnNumericData::kInt, 0);
    dtAttrFn.setNiceNameOverride(dimTargetsAttrName[2]);
    //dtAttrFn.setWritable(false); // 同上
    dtAttrFn.setConnectable(false);
    addAttribute(dtAttr);

    // 入力データ
    MFnNumericAttribute inAttrFn;
    MObject inputAttr = inAttrFn.create(inAttrName[0], inAttrName[1], MFnNumericData::kDouble, 0.0);
    inAttrFn.setNiceNameOverride(inAttrName[2]);
    inAttrFn.setReadable(false);
    inAttrFn.setArray(true);
    inAttrFn.setKeyable(true);
    inAttrFn.setDisconnectBehavior(MFnAttribute::kReset);
    addAttribute(inputAttr);

    // 出力アトリビュートは各モデルの学習後に生成

    // 目標出力データ
    MFnNumericAttribute tgtAttrFn;
    MObject targetAttr = tgtAttrFn.create(targetAttrName[0], targetAttrName[1], MFnNumericData::kDouble, 0.0);
    tgtAttrFn.setNiceNameOverride(targetAttrName[2]);
    tgtAttrFn.setReadable(false);
    tgtAttrFn.setArray(true);
    tgtAttrFn.setKeyable(true);
    addAttribute(targetAttr);

    return MS::kSuccess;
}

/**
 * @fn MStatus mltkNode::setDependentsDirty(const MPlug& plugBeingDirtied, MPlugArray& affectedPlugs)
 * @brief setDependentsDirtyオーバーライド
 * @note 入力＆目標出力アトリビュートの動的変化に対応するためオーバーライド．
 */
MStatus mltkNode::setDependentsDirty(const MPlug& plugBeingDirtied, MPlugArray& affectedPlugs)
{
    MFnDependencyNode thisNodeFn(thisMObject());
    if (!thisNodeFn.hasAttribute(outAttrName[0]))
    {
        // 出力アトリビュート未生成時（未学習時）には計算不要
        return MS::kUnknownParameter;
    }
    // プラグの識別には，アトリビュートの一致判定ではなく，partialNameの文字列比較を用いている．
    //  （例： 入力データInputおよびその要素に対応するpluginBeginDirtied.attribute()と，initNode 内で初期化されるinputAttrアトリビュートは一致しなかった）
    MString partialName = plugBeingDirtied.partialName();
    if (inAttrName[1] == partialName
     || inAttrName[1] != partialName.substring(0, inAttrName[1].length() - 1))
    {
        return MS::kUnknownParameter;
    }

    // 入力プラグ配列の各要素を，全ての出力データに影響させる
    MPlug oplug = thisNodeFn.findPlug(outAttrName[0]);
    for (int i = 0; i < oplug.numElements(); ++i)
    {
        MPlug op = oplug.elementByLogicalIndex(i);
        affectedPlugs.append(op);
    }
    return MS::kSuccess;
}

/**
 * @fn MStatus mltkNode::compute(const MPlug& plug, MDataBlock& data)
 * @brief computeオーバーライド
 */
MStatus mltkNode::compute(const MPlug& plug, MDataBlock& data)
{
    // 出力データプラグ配列の要素だった場合のみ計算対象とする
    MString partialName = plug.partialName();
    if (outAttrName[1] == partialName
     || outAttrName[1] != partialName.substring(0, outAttrName[1].length() - 1))
    {
        return MS::kUnknownParameter;
    }
    return MS::kSuccess;
}

// 選択中のノード群からmltkノードのみを抽出
std::vector<mltkNode*> NodesFromActiveSelection()
{
    MSelectionList asl;
    MGlobal::getActiveSelectionList(asl);
    std::vector<mltkNode*> mltkNodes;
    for (MItSelectionList slit(asl, MFn::kPluginDependNode); !slit.isDone(); slit.next())
    {
        MObject node;
        slit.getDependNode(node);
        MFnDependencyNode nodeFn(node);
        mltkNode* mNode = dynamic_cast<mltkNode*>(nodeFn.userNode());
        if (mNode != nullptr)
        {
            mltkNodes.push_back(mNode);
        }
    }
    return mltkNodes;
}

MStatus mltkNode::preLearn()
{
    return MS::kSuccess;
}

MStatus mltkNode::postLearn()
{
    return MS::kSuccess;
}

/**
 * @fn MStatus LearnMltkNodes::doIt(const MArgList& args)
 * @brief ノード学習コマンド．複数ノードを一括して処理可能．
 */
MStatus LearnMltkNodes::doIt(const MArgList& args)
{
    // フレーム数＝サンプル数
    const MTime startTime = MAnimControl::animationStartTime();
    const MTime endTime = MAnimControl::animationEndTime();
    const int numFrames = static_cast<int>(endTime.value()) - static_cast<int>(startTime.value()) + 1;

    // 選択ノード群からMLTKノードを抽出
    std::vector<mltkNode*> mltkNodes = NodesFromActiveSelection();
    const int numNodes = mltkNodes.size();

    // 学習用データ（Eigen::MatrixXd）の領域確保
    std::vector<Eigen::MatrixXd> inputs, targets;
    for (size_t n = 0; n < mltkNodes.size(); ++n)
    {
        MFnDependencyNode thisNodeFn(mltkNodes[n]->thisMObject());
        const int dimInputs = thisNodeFn.findPlug(mltkNode::inAttrName[0]).numElements();
        thisNodeFn.findPlug(mltkNode::dimInAttrName[0]).setInt(dimInputs);
        inputs.push_back(Eigen::MatrixXd(numFrames, dimInputs));
        const int dimTargets = thisNodeFn.findPlug(mltkNode::targetAttrName[0]).numElements();
        thisNodeFn.findPlug(mltkNode::dimTargetsAttrName[0]).setInt(dimTargets);
        targets.push_back(Eigen::MatrixXd(numFrames, dimTargets));
    }
    // 学習用データの書き込み
    int frame = 0;
    for (MTime time = startTime; time <= endTime; ++time, ++frame)
    {
        MAnimControl::setCurrentTime(time);
        for (int n = 0; n < numNodes; ++n)
        {
            MFnDependencyNode thisNodeFn(mltkNodes[n]->thisMObject());
            MPlug iplug = thisNodeFn.findPlug(mltkNode::inAttrName[0]);
            for (int i = 0; i < inputs[n].cols(); ++i)
            {
                inputs[n](frame, i) = iplug.elementByLogicalIndex(i).asDouble();
            }
			MPlug tplug = thisNodeFn.findPlug(mltkNode::targetAttrName[0]);
            for (int i = 0; i < targets[n].cols(); ++i)
            {
                targets[n](frame, i) = tplug.elementByLogicalIndex(i).asDouble();
            }
        }
    }
    // 各ノードのlearnメソッドを呼び出して学習実行
    for (int n = 0; n < numNodes; ++n)
    {
        mltkNodes[n]->preLearn();
        mltkNodes[n]->learn(inputs[n], targets[n]);
        mltkNodes[n]->postLearn();
    }
    return MS::kSuccess;
}
