#ifndef MLTK_LINEAR_REGRESSION_H
#define MLTK_LINEAR_REGRESSION_H
#pragma once

#include "mltkNode.h"

/**
 * @class mltkLinearRegression mltkLinearRegression.h
 * @brief 線形回帰ノード
 */
class mltkLinearRegression : public mltkNode
{
//
// public static variables
public:
    //! 多項式係数アトリビュート名
    static const MString coefficientName[3];

//
// コンストラクタ&デストラクタ
public:
    mltkLinearRegression();
    virtual ~mltkLinearRegression();

//
// 学習メソッド
public:
    MStatus learn(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output) override;

//
// オーバーライド
public:
    MStatus compute(const MPlug& plug, MDataBlock& data) override;

//
// ノード生成用
public:
    static MStatus initLinearRegression();
    static const MTypeId mltkLinearRegressionID;
};

#endif //MLTK_LINEAR_REGRESSION_H
