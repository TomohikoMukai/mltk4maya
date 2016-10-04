#include "mltkDimensionalityReduction.h"
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MGlobal.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MNodeMessage.h>
#include <Eigen/Dense>

typedef mltkDimensionalityReduction DRN;

const MTypeId DRN::mltkDimensionalityReductionID = 0x00012; // �l�͓K��
const MString DRN::methodTypes[DRN::NumMethods] = {"SVD"};
const MString DRN::methodAttrName[3] = {"method", "method", "Method"};
const MString DRN::codingModes[2] = {"Encode", "Decode"};
const MString DRN::codingModeAttrName[3] = {"codingMode", "cm", "Coding mode"};
const MString DRN::basisAttrName[3] = {"basis", "basis", "Basis"};
const MString DRN::encoderNodeName[3] = {"encoderNode", "en", "Encoder"};
MObject DRN::methodAttr = MObject::kNullObj;
MObject DRN::codingAttr = MObject::kNullObj;
MObject DRN::basisAttr = MObject::kNullObj;
MObject DRN::encoderNodeAttr = MObject::kNullObj;

mltkDimensionalityReduction::mltkDimensionalityReduction()
    : mltkNode()
{
}

mltkDimensionalityReduction::~mltkDimensionalityReduction()
{
}

MStatus mltkDimensionalityReduction::initDimensionalityReduction()
{    
    MStatus status = mltkNode::initNode();
    if (status != MS::kSuccess)
    {
        return status;
    }
    
    // ��@�I��
    MFnEnumAttribute metAttrFn;
    methodAttr = metAttrFn.create(methodAttrName[0], methodAttrName[1]);
    metAttrFn.setNiceNameOverride(methodAttrName[2]);
    for (int i = 0; i < NumMethods; ++i)
    {
        metAttrFn.addField(methodTypes[i], i);
    }
    metAttrFn.setDefault(methodTypes[0]);
    addAttribute(methodAttr);

    // �G���R�[�h or �f�R�[�h
    MFnEnumAttribute codAttrFn;
    codingAttr = codAttrFn.create(codingModeAttrName[0], codingModeAttrName[1]);
    codAttrFn.setNiceNameOverride(codingModeAttrName[2]);
    for (int i = 0; i < 2; ++i)
    {
        codAttrFn.addField(codingModes[i], i);
    }
    codAttrFn.setDefault(codingModes[Encode]);
    addAttribute(codingAttr);

    // ���
    MFnNumericAttribute basAttrFn;
    basisAttr = basAttrFn.create(basisAttrName[0], basisAttrName[1], MFnNumericData::kDouble, 0.0);
    basAttrFn.setNiceNameOverride(basisAttrName[2]);
    basAttrFn.setArray(true);
    basAttrFn.setStorable(true);
    basAttrFn.setConnectable(false);
    addAttribute(basisAttr);

    // �t�ϊ��m�[�h�p�G���R�[�h�m�[�h�ڑ�
    MFnMessageAttribute mAttr;
    encoderNodeAttr = mAttr.create(encoderNodeName[0], encoderNodeName[1]);
    mAttr.setNiceNameOverride(encoderNodeName[2]);
    mAttr.setReadable(false);
    mAttr.setHidden(false);
    addAttribute(encoderNodeAttr);

    return MS::kSuccess;
}

void attributeChangedCallback(MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* p)
{
    // �G���R�[�h�m�[�h�Ƃ̐ڑ����`�F�b�N
    if (plug.isNull())
    {
        return;
    }
    MFnDependencyNode thisNodeFn(plug.node());
    MPlug thisMethodPlug = thisNodeFn.findPlug(DRN::methodAttr);
    MPlug thisCodingPlug = thisNodeFn.findPlug(DRN::codingAttr);
    MPlug thisEncoderNodePlug = thisNodeFn.findPlug(DRN::encoderNodeAttr);
    MPlugArray plugArray;
    if (plug != thisEncoderNodePlug || !thisEncoderNodePlug.connectedTo(plugArray, true, false) || plugArray.length() == 0)
    {
        return;
    }
    // �m�[�h�̎�ʃ`�F�b�N
    MFnDependencyNode srcNodeFn(plugArray[0].node());
    if (!srcNodeFn.hasAttribute(DRN::codingModeAttrName[0])
     && !srcNodeFn.hasAttribute(DRN::methodAttrName[0])
     && !srcNodeFn.hasAttribute(DRN::basisAttrName[0]))
    {
        return;
    }

    // �t�ϊ��p�f�[�^�̐ݒ�
    const int dimInputs = srcNodeFn.findPlug(mltkNode::dimInAttrName[0]).asInt();
    thisNodeFn.findPlug(mltkNode::dimInAttrName[0]).setInt(dimInputs);
    thisNodeFn.findPlug(mltkNode::dimOutAttrName[0]).setInt(dimInputs);
    thisMethodPlug.setShort(srcNodeFn.findPlug(DRN::methodAttr).asShort());
    thisCodingPlug.setShort(DRN::Decode);
    MPlug dstBasisPlug = thisNodeFn.findPlug(DRN::basisAttrName[0]);
    MPlug srcBasisPlug = srcNodeFn.findPlug(DRN::basisAttrName[0]);
    dstBasisPlug.setNumElements(dimInputs * dimInputs);
    for (int i = 0; i < dimInputs; ++i)
    {
        for (int j = 0; j < dimInputs; ++j)
        {
            double a = srcBasisPlug.elementByLogicalIndex(j * dimInputs + i).asDouble();
            dstBasisPlug.elementByLogicalIndex(i * dimInputs + j).setDouble(a);
        }
    }

    // �o�̓|�[�g�̐���
    if (!thisNodeFn.hasAttribute(mltkNode::outAttrName[0]))
    {
        MFnNumericAttribute outAttrFn;
        MObject outAttr = outAttrFn.create(mltkNode::outAttrName[0], mltkNode::outAttrName[1], MFnNumericData::kDouble, 0.0);
        outAttrFn.setNiceNameOverride(mltkNode::outAttrName[2]);
        outAttrFn.setWritable(false);
        outAttrFn.setArray(true);
        thisNodeFn.addAttribute(outAttr);
    }
    thisNodeFn.findPlug(mltkNode::outAttrName[0]).setNumElements(dimInputs);
}

/**
 * @fn void mltkDimensionalityReduction::postConstructor()
 * @brief postConstructor�I�[�o�[���C�h�D�t�ϊ��m�[�h�쐬�p
 */
void mltkDimensionalityReduction::postConstructor()
{
    MNodeMessage::addAttributeChangedCallback(thisMObject(), attributeChangedCallback);
}

/**
 * @fn MStatus mltkDimensionalityReduction::learnSVD(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output)
 * @brief SVD��p�����������팸
 */
MStatus mltkDimensionalityReduction::learnSVD(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output)
{
    MStatus status = MS::kSuccess;
    MFnDependencyNode thisNodeFn(thisMObject());

    const int dimInputs = input.cols();
    const int dimOutputs = output.cols();

    // ���̓f�[�^�ɒ萔����ǉ�
    // ���͍s�� �~ �W���s�� = �o�͍s��@�̘A���������̋���
    auto svd = input.jacobiSvd(Eigen::ComputeThinV);
    Eigen::MatrixXd mv = svd.matrixV();
    Eigen::VectorXd sv = svd.singularValues();
    // ���ْl�̏o��
    char buf[256];
    sprintf(buf, "singular values of %s", name().asChar());
    MGlobal::displayInfo("singular values");
    for (int i = 0; i < sv.size(); ++i)
    {
        sprintf(buf, "%f", sv[i]);
        MGlobal::displayInfo(buf);
    }

    // ���̐ݒ�
    MPlug bplug = thisNodeFn.findPlug(basisAttrName[0], &status);
    bplug.setNumElements(dimInputs * dimInputs);
    for (int i = 0; i < dimInputs; ++i)
    {
        for (int j = 0; j < dimInputs; ++j)
        {
            if (thisNodeFn.findPlug(DRN::codingAttr).asInt() == Encode)
            {
                // �G���R�[�_�ɂ͉E���ٍs��V��ݒ�
                bplug.elementByLogicalIndex(i * dimInputs + j).setDouble(mv(i, j));
            }
            else
            {
                // �f�R�[�_�ɂ͉E���ٍs��̓]�uV'��ݒ�
                bplug.elementByLogicalIndex(i * dimInputs + j).setDouble(mv(j, i));
            }
        }
    }
    return status;
}

MStatus mltkDimensionalityReduction::learn(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output)
{
    MStatus status;
    MFnDependencyNode thisNodeFn(thisMObject());

    const int dimInputs = input.cols();
    const int method = thisNodeFn.findPlug(DRN::methodAttrName[0]).asInt();

    switch (method)
    {
    case MethodSVD:
        status = learnSVD(input, output);
        break;
    default:
        return MS::kInvalidParameter;
        break;
    }

    // �o�̓f�[�^�A�g���r���[�g�̐���
    if (!thisNodeFn.hasAttribute(outAttrName[0]))
    {
        MFnNumericAttribute nAttr;
        MObject outAttr = nAttr.create(outAttrName[0], outAttrName[1], MFnNumericData::kDouble, 0.0);
        nAttr.setNiceNameOverride(outAttrName[2]);
        nAttr.setWritable(false);
        nAttr.setArray(true);
        thisNodeFn.addAttribute(outAttr);
    }
    thisNodeFn.findPlug(outAttrName[0]).setNumElements(dimInputs);
    thisNodeFn.findPlug(dimOutAttrName[0]).setInt(dimInputs);

    return MS::kSuccess;
}

MStatus mltkDimensionalityReduction::compute(const MPlug& plug, MDataBlock& data)
{
    MStatus status = mltkNode::compute(plug, data);
    if (status != MS::kSuccess)
    {
        return status;
    }

    MFnDependencyNode thisNode(thisMObject());
    const int plugId = plug.logicalIndex();
    const int dimInputs = data.inputValue(thisNode.findPlug(dimInAttrName[0])).asInt();
    if (plugId > dimInputs)
    {
        return MS::kUnknownParameter;
    }

    MPlug iplug = thisNode.findPlug(inAttrName[0]);
    MPlug bplug = thisNode.findPlug(basisAttrName[0]);
    double sum = 0.0;
    for (int j = 0; j < dimInputs; ++j)
    {
        MStatus istat, bstat;
        MDataHandle inputHandle = data.inputValue(iplug.elementByLogicalIndex(j), &istat);
        MDataHandle basisHandle = data.inputValue(bplug.elementByLogicalIndex(j * dimInputs + plugId), &bstat);
        if (istat == MS::kSuccess && bstat == MS::kSuccess)
        {
            sum += basisHandle.asDouble() * inputHandle.asDouble();
        }
    }
    data.outputValue(plug).setDouble(sum);
    data.setClean(plug);

    return MS::kSuccess;
}
