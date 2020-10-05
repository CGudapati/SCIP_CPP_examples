#include <iostream>
#include <sstream>
#include "sudoku_utils.h"
#include <scip/scip.h>
#include <scip/scipdefplugins.h>

int main(int args, char *argv[])
{
    if (args < 2)
    {
        std::cerr << "call " << argv[0] << " <puzzle file> " << "\n";
        exit(1);
    }

    std::string puzzle_file_path = argv[1];

    auto puzzle = sudoku::get_sudoku_grid(puzzle_file_path);

    std::cout << "The unsolved Sudoku Puzzle is: " << "\n";
    sudoku::print_sudoku(puzzle);

    //Setting up the SCIP environment
    
    SCIP *scip = nullptr; //Declaring the scip environment

    SCIP_CALL(SCIPcreate(&scip)); //Creating the SCIP environment

    /* include default plugins */
    SCIP_CALL(SCIPincludeDefaultPlugins(scip));

    //Creating the SCIP Problem.
    SCIP_CALL(SCIPcreateProbBasic(scip, "SUDOKU"));

    SCIP_CALL(SCIPsetObjsense(scip, SCIP_OBJSENSE_MINIMIZE)); //Does not matter for us as this is just a feasibility problem


    /*
     * To solve the puzzle using integer programming, we shall use this handy tutorial;
     * http://profs.sci.univr.it/~rrizzi/classes/PLS2015/sudoku/doc/497_Olszowy_Wiktor_Sudoku.pdf
     * prior knowledge of sudoku is presumed.
    */

    /*
    * We have to define 9x9x9 variables. Let x_{ijk} where i = 1...9, j = 1...9 and k = 1..9 be those binary variables.
    * x_{ijk} is the the  binary variable related to number k (1 or 2 or ... 9) in the ith row and jth column. 
    * 
   */



    std::vector<std::vector<std::vector<SCIP_VAR *>>> x_vars(9, std::vector<std::vector<SCIP_VAR *>>(9, std::vector<SCIP_VAR *>(9)));

    std::ostringstream namebuf;

    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            for (int k = 0; k < 9; ++k)
            {
                SCIP_VAR *var = nullptr;
                namebuf.str("");
                namebuf << "x[" << i << "," << j << "," << k << "]";
                SCIP_CALL(SCIPcreateVarBasic(scip,                  // SCIP environment
                                             &var,                  // reference to the variable
                                             namebuf.str().c_str(), // name of the variable
                                             0.0,                   // Lower bound of the variable
                                             1.0,                   // upper bound of the variable
                                             1.0,                   // Obj. coefficient. Doesn't really matter for this problem
                                             SCIP_VARTYPE_BINARY    // Binary variable
                                             ));
                SCIP_CALL(SCIPaddVar(scip, var));
                x_vars[i][j][k] = var;
            }
        }
    }

    // Let us create the constraints now. Since there is nothing advanced about our constraints,
    // we will just use the simple constraints given by SCIPcreateConsBasicLinear()



    std::vector<SCIP_CONS *> column_constrs;   //These constraints will model that fact that in each column, the numbers 1..9 should not repeat. 

    for (int j = 0; j < 9; ++j)
    {
        for (int k = 0; k < 9; ++k)
        {
            SCIP_CONS *cons = nullptr;

            namebuf.str("");
            namebuf << "col_" << j << "_" << k << "]";
            SCIP_CALL(SCIPcreateConsBasicLinear(scip,
                                                &cons,
                                                namebuf.str().c_str(),
                                                0,
                                                nullptr,
                                                nullptr,
                                                1.0,
                                                1.0));
            for (int i = 0; i < 9; ++i)            // The constraint will look like x_1jk + x_2jk + x_3jk + ... + x_9jk = 1 for a given value of j and k
            {
                SCIP_CALL(SCIPaddCoefLinear(scip, cons, x_vars[i][j][k], 1.0));
            }

            SCIP_CALL(SCIPaddCons(scip, cons));
            column_constrs.push_back(cons);
        }
    }

    std::vector<SCIP_CONS *> row_constrs;  //These constraints will model that fact that in each row, the numbers 1..9 do not repeat. 

    for (int i = 0; i < 9; ++i)
    {
        for (int k = 0; k < 9; ++k)
        {
            SCIP_CONS *cons = nullptr;

            namebuf.str("");
            namebuf << "row_" << i << "_" << k << "]";
            SCIP_CALL(SCIPcreateConsBasicLinear(scip,
                                                &cons,
                                                namebuf.str().c_str(),
                                                0,
                                                nullptr,
                                                nullptr,
                                                1.0,
                                                1.0));
            for (int j = 0; j < 9; ++j)          // The constraint will look like x_i1k + x_i2k + x_i3k + ... + x_i9k = 1 for a given value of i and k
            {
                SCIP_CALL(SCIPaddCoefLinear(scip, cons, x_vars[i][j][k], 1.0));
            }

            SCIP_CALL(SCIPaddCons(scip, cons));
            row_constrs.push_back(cons);
        }
    }

    //Subgrid constraints
    std::vector<SCIP_CONS *> subgrid_constrs;  // These constraints will model that each of the 3x3 subgrids will contain 1...9 without any repetition. 

    for (int k = 0; k < 9; ++k)
    {
        for (int p = 0; p < 3; ++p)
        {
            for (int q = 0; q < 3; ++q)
            {
                SCIP_CONS *cons = nullptr;

                namebuf.str("");
                namebuf << "subgrid_" << k << "_" << p << "_" << q << "]";
                SCIP_CALL(SCIPcreateConsBasicLinear(scip,
                                                    &cons,
                                                    namebuf.str().c_str(),
                                                    0,
                                                    nullptr,
                                                    nullptr,
                                                    1.0,
                                                    1.0));
                for (int j = 3 * (p + 1) - 3; j < 3 * (p + 1); ++j)                    //since we are indexing from 0..8 be careful with the loop indices. 
                {
                    for (int i = 3 * (q + 1) - 3; i < 3 * (q + 1); ++i)
                    {
                        SCIP_CALL(SCIPaddCoefLinear(scip, cons, x_vars[i][j][k], 1.0));
                    }
                }
                SCIP_CALL(SCIPaddCons(scip, cons));
                subgrid_constrs.push_back(cons);
            }
        }
    }

    //ensure that the complete puzzle grid is filled constraints     
    std::vector<SCIP_CONS *> fillgrid_constrs;        //These constraints ensure that every position in the whole 9x9 grid is filled with one number.

    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            SCIP_CONS *cons = nullptr;

            namebuf.str("");
            namebuf << "fillgrid_" << i << "_" << j << "]";
            SCIP_CALL(SCIPcreateConsBasicLinear(scip,
                                                &cons,
                                                namebuf.str().c_str(),
                                                0,
                                                nullptr,
                                                nullptr,
                                                1.0,
                                                1.0));
            for (int k = 0; k < 9; ++k)
            {
                SCIP_CALL(SCIPaddCoefLinear(scip, cons, x_vars[i][j][k], 1.0));
            }

            SCIP_CALL(SCIPaddCons(scip, cons));
            fillgrid_constrs.push_back(cons);
        }
    }

    //We have to set assign the already given numbers to the corresponding variables and we use the SCIPfixVar() function

    SCIP_Bool infeasible;
    SCIP_Bool fixed;
    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            if (puzzle[i][j] > 0)
            {
                SCIP_CALL(SCIPfixVar(scip, x_vars[i][j][(puzzle[i][j]) - 1], 1.0, &infeasible, &fixed));
            }
        }
    }

    SCIP_CALL(SCIPsetObjsense(scip, SCIP_OBJSENSE_MAXIMIZE));

    // SCIP_CALL((SCIPwriteOrigProblem(scip, "scip_sudoku.lp", nullptr, FALSE)));

    SCIP_CALL(SCIPsetIntParam(scip, "display/verblevel", 0));   // We use SCIPsetIntParams to turn off the logging. 

    SCIP_CALL(SCIPsolve(scip));

    SCIP_STATUS soln_status = SCIPgetStatus(scip);   // Some wrongly generated sudoku puzzles can be infeasible. So we use the solnstatus to display different types of output.

    if (soln_status == 11)                           // solution status of 11 indicates optimal solution was found. Hence we can print the final puzzle.
    {
        SCIP_SOL *sol;
        sol = SCIPgetBestSol(scip);

        for (int i = 0; i < 9; ++i)
        {
            for (int j = 0; j < 9; ++j)
            {
                for (int k = 0; k < 9; ++k)
                {
                    if (SCIPgetSolVal(scip, sol, x_vars[i][j][k]) > 0)
                    {
                        puzzle[i][j] = k + 1;     // We are using 0  based indices. so when we have to display the final puzzle, we should increment it by 1.
                    }
                }
            }
        }
        std::cout << "The solved puzzle is: " << "\n";
        sudoku::print_sudoku(puzzle);
    }
    else if( soln_status == 12)                    // solutions status of 12 indicates that the puzzle is infeasible. 
    {
        std::cout << "Check the Input puzzle" << "\n";
    }

    //Freeing the variables

    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            for (int k = 0; k < 9; ++k)
            {
                SCIP_CALL(SCIPreleaseVar(scip, &x_vars[i][j][k]));
            }
        }
    }
    x_vars.clear();

    // Freeing the constraints

    for (auto &constr : column_constrs)
    {
        SCIP_CALL(SCIPreleaseCons(scip, &constr));
    }
    column_constrs.clear();

    for (auto &constr : row_constrs)
    {
        SCIP_CALL(SCIPreleaseCons(scip, &constr));
    }
    row_constrs.clear();

    for (auto &constr : subgrid_constrs)
    {
        SCIP_CALL(SCIPreleaseCons(scip, &constr));
    }
    subgrid_constrs.clear();

    for (auto &constr : fillgrid_constrs)
    {
        SCIP_CALL(SCIPreleaseCons(scip, &constr));
    }
    fillgrid_constrs.clear();

    //freeing scip

    SCIP_CALL(SCIPfree(&scip));
}
