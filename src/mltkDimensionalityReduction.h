#ifndef MLTK_DIMENSIONALITY_REDUCTION_NODE_H
#define MLTK_DIMENSIONALITY_REDUCTION_NODE_H
#pragma once

#include "mltkNode.h"

/**
 * @class mltkDimensionalityReduction mltkDimensionalityReduction.h
 * @brief �������팸�m�[�h
 */
class mltkDimensionalityReduction : public mltkNode
{
//
// public static variables
public:
    enum Method { MethodSVD, NumMethods };
    enum Coding { Encode, Decode };
    static const MString methodTypes[NumMethods];
    static const MString codingModes[2];

    static const MString methodAttrName[3];
    static const MString codingModeAttrName[3];
    static const MString basisAttrName[3];
    static const MString encoderNodeName[3];
    static MObject methodAttr;
    static MObject codingAttr;
    static MObject basisAttr;
    static MObject encoderNodeAttr;

//
// �R���X�g���N�^&�f�X�g���N�^
public:
    mltkDimensionalityReduction();
    virtual ~mltkDimensionalityReduction();

//
// �w�K���\�b�h
public:
    MStatus learn(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output) override;
    MStatus learnSVD(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output);

//
// �I�[�o�[���C�h
public:
    void postConstructor() override;
    MStatus compute(const MPlug& plug, MDataBlock& data) override;

//
// �m�[�h�����p
public:
    static MStatus initDimensionalityReduction();
    static const MTypeId mltkDimensionalityReductionID;
};

#endif //MLTK_DIMENSIONALITY_REDUCTION_NODE_H
