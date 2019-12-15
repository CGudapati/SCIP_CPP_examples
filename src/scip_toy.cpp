//
//  scip_toy.cpp
//  SCIP_toyeg
//
//  Created by Naga V Gudapati on 12/13/19.
//  Copyright © 2019 Naga V Gudapati. All rights reserved.
//

#include <iostream>
#include <scip/scip.h>
#include <scip/scipdefplugins.h>

SCIP_RETCODE execmain(int argc, const char** argv)
{
    SCIP* scip = nullptr;
    SCIP_CALL( SCIPcreate(&scip) );  //Creating the SCIP environment

    
    /* include default plugins */
    SCIP_CALL( SCIPincludeDefaultPlugins(scip) );
    
    //Creating the SCIP Problem.
    SCIP_CALL( SCIPcreateProbBasic(scip, "SCIP_toy_example"));
    
    SCIP_CALL(SCIPsetObjsense(scip, SCIP_OBJSENSE_MAXIMIZE));

    
    SCIP_VAR* x1 = nullptr;
    
    SCIP_CALL( SCIPcreateVarBasic(scip,
    &x1,                     // returns new index
    "x1",                      // name
    0.0,                      // lower bound
    SCIPinfinity(scip),       // upper bound
    3.0,                      // objective
    SCIP_VARTYPE_CONTINUOUS));  // variable type
                                
    
    SCIP_CALL( SCIPaddVar(scip, x1) );  //Adding the first variable
    
    SCIP_VAR* x2 = nullptr;;
    
    SCIP_CALL( SCIPcreateVarBasic(scip,
    &x2,                     // returns new index
    "x2",                      // name
    0.0,                      // lower bound
    SCIPinfinity(scip),       // upper bound
    2.0,                      // objective
    SCIP_VARTYPE_CONTINUOUS));  // variable type
    
    SCIP_CALL( SCIPaddVar(scip, x2) ); //Adding the second variable
    
    SCIP_CONS* cons1 = nullptr;;
    
    
    SCIP_CALL(SCIPcreateConsBasicLinear(scip,                 // SCIP pointer
                                        &cons1,               // pointer to SCIP constraint
                                        "cons1",              // name of the constraint
                                        0,                    // How many variables are you adding  now
                                        nullptr,              // an array of pointers to various variables
                                        nullptr,              // an array of values of the coefficients of the corresponding vars
                                        -SCIPinfinity(scip),  // LHS of the constraint
                                        4));                  // RHS of the constraint
    
    SCIP_CALL( SCIPaddCoefLinear(scip, cons1, x1, 1.0));     //Adding the variable x1 with A matrix coeffient of 1.0
    SCIP_CALL( SCIPaddCoefLinear(scip, cons1, x2, 1.0));     //Adding the variable x2 with A matrix coeffient of 1.0
    SCIP_CALL( SCIPaddCons(scip, cons1));
    
        
    
    SCIP_CONS* cons2 = nullptr;;
    
    SCIP_CALL(SCIPcreateConsBasicLinear(scip,
                                        &cons2,
                                        "cons2",
                                        0,
                                        nullptr,
                                        nullptr,
                                        -SCIPinfinity(scip),
                                        5));
    
    SCIP_CALL( SCIPaddCoefLinear(scip, cons2, x1, 2.0));
    SCIP_CALL( SCIPaddCoefLinear(scip, cons2, x2, 1.0));
    SCIP_CALL( SCIPaddCons(scip, cons2));
    

    
    SCIP_CONS* cons3 = nullptr;;
    
    SCIP_CALL(SCIPcreateConsBasicLinear(scip,
                                        &cons3,
                                        "cons3",
                                        0,
                                        nullptr,
                                        nullptr,
                                        -SCIPinfinity(scip),
                                        -2.0));
    
    SCIP_CALL( SCIPaddCoefLinear(scip, cons3, x1, 1.0));
    SCIP_CALL( SCIPaddCoefLinear(scip, cons3, x2, -4.0));
    SCIP_CALL( SCIPaddCons(scip, cons3));

    
    //Scip releasing all the constraints
    SCIP_CALL( SCIPreleaseCons(scip, &cons1) );
    SCIP_CALL( SCIPreleaseCons(scip, &cons2) );
    SCIP_CALL( SCIPreleaseCons(scip, &cons3) );
    
    //Solving the problem
    SCIP_CALL( SCIPsolve(scip) );
    
    SCIP_SOL* sol = nullptr;
    
    sol = SCIPgetBestSol(scip);
    
    std::cout << "x1: " << SCIPgetSolVal(scip, sol, x1) << " " << "x2: " << SCIPgetSolVal(scip, sol, x2) << "\n";
    

    SCIP_CALL( (SCIPwriteOrigProblem(scip, "scip_toy.lp", nullptr, FALSE)));
    
    


    //Freeing the variables
    SCIP_CALL( SCIPreleaseVar(scip, &x1));

    SCIP_CALL( SCIPreleaseVar(scip, &x2));
    


    SCIP_CALL( SCIPfree(&scip));
    
        
    return SCIP_OKAY;
    
}


int main(int argc, const char * argv[]) {
    
    return execmain(argc, argv) != SCIP_OKAY ? 1 : 0;
    
}
