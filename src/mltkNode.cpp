#include "mltkNode.h"
#include <vector>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#include <maya/MItSelectionList.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MAnimControl.h>
#include <Eigen/Core>

const MTypeId mltkNode::mltkNodeID = 0x00010; // �l�͓K���ɐݒ�
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
	// ���͎�����
    MFnNumericAttribute diAttrFn;
    MObject diAttr = diAttrFn.create(dimInAttrName[0], dimInAttrName[1], MFnNumericData::kInt, 0);
    diAttrFn.setNiceNameOverride(dimInAttrName[2]);
    //diAttrFn.setWritable(false); // ���[�U�[�ҏW�͋��������Ȃ����C�t�@�C���ۑ�����Ȃ��Ȃ邽�߃R�����g�A�E�g
    diAttrFn.setStorable(true);
    diAttrFn.setConnectable(false);
    addAttribute(diAttr);

    // �o�͎�����
    MFnNumericAttribute doAttrFn;
    MObject doAttr = doAttrFn.create(dimOutAttrName[0], dimOutAttrName[1], MFnNumericData::kInt, 0);
    doAttrFn.setNiceNameOverride(dimOutAttrName[2]);
    //doAttrFn.setWritable(false); // ����
    doAttrFn.setConnectable(false);
    addAttribute(doAttr);

    // �ڕW�o�͎�����
    MFnNumericAttribute dtAttrFn;
    MObject dtAttr = dtAttrFn.create(dimTargetsAttrName[0], dimTargetsAttrName[1], MFnNumericData::kInt, 0);
    dtAttrFn.setNiceNameOverride(dimTargetsAttrName[2]);
    //dtAttrFn.setWritable(false); // ����
    dtAttrFn.setConnectable(false);
    addAttribute(dtAttr);

    // ���̓f�[�^
    MFnNumericAttribute inAttrFn;
    MObject inputAttr = inAttrFn.create(inAttrName[0], inAttrName[1], MFnNumericData::kDouble, 0.0);
    inAttrFn.setNiceNameOverride(inAttrName[2]);
    inAttrFn.setReadable(false);
    inAttrFn.setArray(true);
    inAttrFn.setKeyable(true);
    inAttrFn.setDisconnectBehavior(MFnAttribute::kReset);
    addAttribute(inputAttr);

    // �o�̓A�g���r���[�g�͊e���f���̊w�K��ɐ���

    // �ڕW�o�̓f�[�^
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
 * @brief setDependentsDirty�I�[�o�[���C�h
 * @note ���́��ڕW�o�̓A�g���r���[�g�̓��I�ω��ɑΉ����邽�߃I�[�o�[���C�h�D
 */
MStatus mltkNode::setDependentsDirty(const MPlug& plugBeingDirtied, MPlugArray& affectedPlugs)
{
    MFnDependencyNode thisNodeFn(thisMObject());
    if (!thisNodeFn.hasAttribute(outAttrName[0]))
    {
        // �o�̓A�g���r���[�g���������i���w�K���j�ɂ͌v�Z�s�v
        return MS::kUnknownParameter;
    }
    // �v���O�̎��ʂɂ́C�A�g���r���[�g�̈�v����ł͂Ȃ��CpartialName�̕������r��p���Ă���D
    //  �i��F ���̓f�[�^Input����т��̗v�f�ɑΉ�����pluginBeginDirtied.attribute()�ƁCinitNode ���ŏ����������inputAttr�A�g���r���[�g�͈�v���Ȃ������j
    MString partialName = plugBeingDirtied.partialName();
    if (inAttrName[1] == partialName
     || inAttrName[1] != partialName.substring(0, inAttrName[1].length() - 1))
    {
        return MS::kUnknownParameter;
    }

    // ���̓v���O�z��̊e�v�f���C�S�Ă̏o�̓f�[�^�ɉe��������
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
 * @brief compute�I�[�o�[���C�h
 */
MStatus mltkNode::compute(const MPlug& plug, MDataBlock& data)
{
    // �o�̓f�[�^�v���O�z��̗v�f�������ꍇ�̂݌v�Z�ΏۂƂ���
    MString partialName = plug.partialName();
    if (outAttrName[1] == partialName
     || outAttrName[1] != partialName.substring(0, outAttrName[1].length() - 1))
    {
        return MS::kUnknownParameter;
    }
    return MS::kSuccess;
}

// �I�𒆂̃m�[�h�Q����mltk�m�[�h�݂̂𒊏o
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
 * @brief �m�[�h�w�K�R�}���h�D�����m�[�h���ꊇ���ď����\�D
 */
MStatus LearnMltkNodes::doIt(const MArgList& args)
{
    // �t���[�������T���v����
    const MTime startTime = MAnimControl::animationStartTime();
    const MTime endTime = MAnimControl::animationEndTime();
    const int numFrames = static_cast<int>(endTime.value()) - static_cast<int>(startTime.value()) + 1;

    // �I���m�[�h�Q����MLTK�m�[�h�𒊏o
    std::vector<mltkNode*> mltkNodes = NodesFromActiveSelection();
    const int numNodes = mltkNodes.size();

    // �w�K�p�f�[�^�iEigen::MatrixXd�j�̗̈�m��
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
    // �w�K�p�f�[�^�̏�������
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
    // �e�m�[�h��learn���\�b�h���Ăяo���Ċw�K���s
    for (int n = 0; n < numNodes; ++n)
    {
        mltkNodes[n]->preLearn();
        mltkNodes[n]->learn(inputs[n], targets[n]);
        mltkNodes[n]->postLearn();
    }
    return MS::kSuccess;
}
