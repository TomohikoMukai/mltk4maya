#ifndef MLTK_LINEAR_REGRESSION_H
#define MLTK_LINEAR_REGRESSION_H
#pragma once

#include "mltkNode.h"

/**
 * @class mltkLinearRegression mltkLinearRegression.h
 * @brief ���`��A�m�[�h
 */
class mltkLinearRegression : public mltkNode
{
//
// public static variables
public:
    //! �������W���A�g���r���[�g��
    static const MString coefficientName[3];

//
// �R���X�g���N�^&�f�X�g���N�^
public:
    mltkLinearRegression();
    virtual ~mltkLinearRegression();

//
// �w�K���\�b�h
public:
    MStatus learn(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output) override;

//
// �I�[�o�[���C�h
public:
    MStatus compute(const MPlug& plug, MDataBlock& data) override;

//
// �m�[�h�����p
public:
    static MStatus initLinearRegression();
    static const MTypeId mltkLinearRegressionID;
};

#endif //MLTK_LINEAR_REGRESSION_H
