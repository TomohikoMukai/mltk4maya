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
 * @brief mltkベースノード
 */
class mltkNode : public MPxNode
{
//
// アトリビュート名
public:
    //! 入力アトリービュート名
	static const MString inAttrName[3];
    //! 出力アトリービュート名
	static const MString outAttrName[3];
    //! 目標出力アトリービュート名
	static const MString targetAttrName[3];
    //! 入力次元数アトリビュート名
	static const MString dimInAttrName[3];
    //! 出力次元数アトリビュート名
	static const MString dimOutAttrName[3];
    //! 目標出力次元数アトリビュート名
	static const MString dimTargetsAttrName[3];

//
// コンストラクタ&デストラクタ
public:
    mltkNode();
    virtual ~mltkNode();

//
// 学習メソッド（各モデルでオーバーライドして実装）
public:
    /**
     * @fn virtual MStatus mltkNode::learn(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output)
     * @brief 学習メソッド
     * @param input 入力データ行列（サンプル数×入力次元数）
     * @param output 入力データ行列（サンプル数×出力次元数）
     * @retval 戻り値
     */
    virtual MStatus learn(const Eigen::MatrixXd& input, const Eigen::MatrixXd& output) = 0;
    // 学習実行直前・直後に実行するメソッド
    virtual MStatus preLearn();
    virtual MStatus postLearn();

//
// オーバーライド
public:
    MStatus setDependentsDirty(const MPlug& plugBeingDirtied, MPlugArray& affectedPlugs) override;
    MStatus compute(const MPlug& plug, MDataBlock& data) override;

//
// ノード生成用
public:
    //! ノード初期化関数
    static MStatus initNode();
    //! ノードID
    static const MTypeId mltkNodeID;
};

/**
 * @class LearnMltkNode mltkNode.h
 * @brief MLTKノード学習コマンド
 */
class LearnMltkNodes : public MPxCommand
{
public:
    virtual MStatus doIt(const MArgList& args);
};

#endif // MLTK_NODE_H
