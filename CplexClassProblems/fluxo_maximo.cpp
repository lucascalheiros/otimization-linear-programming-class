#include "fluxo_maximo.hpp"

typedef IloArray<IloNumVarArray> NumVarMatrix;
typedef IloArray<IloExprArray> ExprMatrix;


using namespace std;


void fluxoSolver() {
    IloEnv env;
    try {
        int ductDim = 5;
        int startingFlux = 20;
        int capacity[5][5] = {  {0, 5, 2, 3, 1},
                                {0, 0, 3, 0, 2},
                                {3, 2, 0, 1, 10},
                                {0, 0, 1, 0, 2},
                                {0, 0, 0, 0, 0}};

        IloModel fluxo(env, "Problema do Fluxo");

        IloCplex cplex(fluxo);

		NumVarMatrix x(env, ductDim);
		ExprMatrix capacityExpr(env, ductDim);

        for (int i = 0; i < ductDim; ++i) {
            x[i] = IloNumVarArray(env, ductDim, 0, IloInfinity);
            capacityExpr[i] = IloExprArray(env, ductDim);
            for (int j = 0; j < ductDim; j++) {
                capacityExpr[i][j] = IloExpr(env);
            }
        }
        IloExprArray entryFlux(env, ductDim);
        IloExprArray outFlux(env, ductDim);

		for (int i = 0; i < ductDim; i++) {
            entryFlux[i] = IloExpr(env);
            outFlux[i] = IloExpr(env);
        }


        for (int i = 0; i < ductDim; i++) {
            for (int j = 0; j < ductDim; j++) {
                entryFlux[i] += x[j][i];
                outFlux[i] += x[i][j];
            }
        }

        // Conservação de fluxo
        for (int i = 1; i < ductDim - 1; i++) {
            fluxo.add(entryFlux[i] == outFlux[i]);
        }

        // Fluxo inicial no entrave 0
        fluxo.add(entryFlux[0] <= startingFlux);

        // Maximizar o fluxo no entrave final
        fluxo.add(IloMaximize(env, entryFlux[4]));

        // Limitar a capacidade de cada duto
        for (int i = 0; i < ductDim; ++i) {
            for (int j = 0; j < ductDim; j++) {
                capacityExpr[i][j] += x[i][j];
                fluxo.add(capacityExpr[i][j] <= capacity[i][j]);
            }
        }

        if (cplex.solve())
            cout << "Fluxo máximo " << cplex.getObjValue() << endl;

		IloArray<IloNumArray> sol(env, ductDim);
		for (int i = 0; i < ductDim; ++i) {
            sol[i] = IloNumArray(env, ductDim);
            cplex.getValues(sol[i], x[i]);
        }

		for (int i = 0; i < ductDim; i++) {
            for (int j = 0; j < ductDim; j++)
                cout << "Duto " << i+1 << " " << j+1 << ": " << sol[i][j] << endl;

		}
    }
	catch (const IloException& e)
	{
		cerr << "Exception caught: " << e << endl;
	}
	catch (...)
	{
		cerr << "Unknown exception caught!" << endl;
	}

	env.end();
	return;
}


