#ifndef MLTK_NODE_H
#define MLTK_NODE_H
#pragma once

#include <maya/MPxNode.h>
#include <maya/MPxCommand.h>
#include <maya/MString.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <Eigen/Core>
#include <map>

/**
 * @class mltkNode mltkNode.h
 * @brief mltk�x�[�X�m�[�h
 */
class mltkNode : public MPxNode
{
//
// �A�g���r���[�g��
public:
    //! ���̓A�g���[�r���[�g��
	static const MString inAttrName[3];
    //! �o�̓A�g���[�r���[�g��
	static const MString outAttrName[3];
    //! �ڕW�o�̓A�g���[�r���[�g��
	static const MString targetAttrName[3];
    //! ���͎������A�g���r���[�g��
	static const MString dimInAttrName[3];
    //! �o�͎������A�g���r���[�g��
	static const MString dimOutAttrName[3];
    //! �ڕW�o�͎������A�g���r���[�g��
	static const MString dimTargetsAttrName[3];

//
// �R���X�g���N�^&�f�X�g���N�^
public:
    mltkNode();
    virtual ~mltkNode();

//
// �w�K���\�b�h�i�e���f���ŃI�[�o�[���C�h���Ď����j
public:
    /**
     * @fn virtual MStatus mltkNode::learn(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output)
     * @brief �w�K���\�b�h
     * @param input ���̓f�[�^�s��i�T���v�����~���͎������j
     * @param output ���̓f�[�^�s��i�T���v�����~�o�͎������j
     * @retval �߂�l
     */
    virtual MStatus learn(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output) = 0;
    // �w�K���s���O�E����Ɏ��s���郁�\�b�h
    virtual MStatus preLearn();
    virtual MStatus postLearn();

//
// �I�[�o�[���C�h
public:
    MStatus setDependentsDirty(const MPlug& plugBeingDirtied, MPlugArray& affectedPlugs) override;
    MStatus compute(const MPlug& plug, MDataBlock& data) override;

//
// �m�[�h�����p
public:
    //! �m�[�h�������֐�
    static MStatus initNode();
    //! �m�[�hID
    static const MTypeId mltkNodeID;
};

/**
 * @class LearnMltkNode mltkNode.h
 * @brief MLTK�m�[�h�w�K�R�}���h
 */
class LearnMltkNodes : public MPxCommand
{
public:
    virtual MStatus doIt(const MArgList& args);
};

#endif // MLTK_NODE_H
