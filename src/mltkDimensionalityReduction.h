#ifndef MLTK_DIMENSIONALITY_REDUCTION_NODE_H
#define MLTK_DIMENSIONALITY_REDUCTION_NODE_H
#pragma once

#include "mltkNode.h"

/**
 * @class mltkDimensionalityReduction mltkDimensionalityReduction.h
 * @brief 次元数削減ノード
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
// コンストラクタ&デストラクタ
public:
    mltkDimensionalityReduction();
    virtual ~mltkDimensionalityReduction();

//
// 学習メソッド
public:
    MStatus learn(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output) override;
    MStatus learnSVD(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output);

//
// オーバーライド
public:
    void postConstructor() override;
    MStatus compute(const MPlug& plug, MDataBlock& data) override;

//
// ノード生成用
public:
    static MStatus initDimensionalityReduction();
    static const MTypeId mltkDimensionalityReductionID;
};

#endif //MLTK_DIMENSIONALITY_REDUCTION_NODE_H
