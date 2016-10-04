#include "mltkNode.h"
#include "mltkLinearRegression.h"
#include "mltkDimensionalityReduction.h"
#include <maya/MFnPlugin.h>

MStatus initializePlugin(MObject obj)
{ 
    MStatus status;
    MFnPlugin plugin(obj, "Mukai Lab" , "2016.10.04", "2016");

    status = plugin.registerNode("mltkLinearRegression", mltkLinearRegression::mltkLinearRegressionID,
        []()->void*{return new mltkLinearRegression();},
        mltkLinearRegression::initLinearRegression);
    CHECK_MSTATUS(status);

    status = plugin.registerNode("mltkDimensionalityReduction", mltkDimensionalityReduction::mltkDimensionalityReductionID,
        []()->void*{return new mltkDimensionalityReduction();},
        mltkDimensionalityReduction::initDimensionalityReduction);
    CHECK_MSTATUS(status);

    status = plugin.registerCommand("LearnMltkNodes",
        []()->void*{return new LearnMltkNodes();});
    CHECK_MSTATUS(status);

    status = plugin.registerUI("mltkCreateMenu", "mltkDeleteMenu");
    CHECK_MSTATUS(status);

    return status;
}

MStatus uninitializePlugin(MObject obj)
{
    MStatus status;
    MFnPlugin plugin(obj);
    status = plugin.deregisterNode(mltkLinearRegression::mltkLinearRegressionID);
    CHECK_MSTATUS(status);
    status = plugin.deregisterNode(mltkDimensionalityReduction::mltkDimensionalityReductionID);
    CHECK_MSTATUS(status);
    status = plugin.deregisterCommand("LearnMltkNodes");
    CHECK_MSTATUS(status);
    return status;
}
