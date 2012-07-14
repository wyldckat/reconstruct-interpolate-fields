/*---------------------------------------------------------------------------* \
License
    This file is designed to work with OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    reconstructSurfaceField

Description
    Reconstruct surfaceScalarField to volVectorField or surfaceVectorField
    to volTensorField.

Author
    wyldckat@github

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "OSspecific.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
//  Main program:

int main(int argc, char *argv[])
{

    timeSelector::addOptions();

    argList::addNote
    (
        "Reconstruct surfaceScalarField to volVectorField or surfaceVectorField to volTensorField."
    );
    argList::addOption
    (
        "scalar",
        "surfaceScalarField",
        "the name of the surfaceScalarField"
    );
    argList::addOption
    (
        "vector",
        "surfaceVectorField",
        "the name of the surfaceVectorField"
    );
    argList::validArgs.append
    (
      "outputVol*Field"
    );

#   include "setRootCase.H"
#   include "createTime.H"

    const fileName volFieldName = args[1];
    
    fileName scalarFieldName, vectorFieldName;
    bool scalarBool = args.optionReadIfPresent("scalar", scalarFieldName);
    bool vectorBool = args.optionReadIfPresent("vector", vectorFieldName);

    if(scalarBool && vectorBool)
    {
        FatalErrorIn(args.executable())
            << "Pick only one type of surface field, either scalar or vector, not both."
            << exit(FatalError);
    }
    else if(!scalarBool && !vectorBool)
    {
        FatalErrorIn(args.executable())
            << "Pick one type of surface field, -scalar or -vector."
            << exit(FatalError);
    }
    
    if(scalarBool)
    {
        if (scalarFieldName == volFieldName)
        {
            FatalErrorIn(args.executable())
                << "Volume field cannot have the same name as the surface field."
                << exit(FatalError);
        }
    }
    else if(vectorBool)
    {
        if (vectorFieldName == volFieldName)
        {
            FatalErrorIn(args.executable())
                << "Volume field cannot have the same name as the surface field."
                << exit(FatalError);
        }
    }
    
    instantList timeDirs = timeSelector::select0(runTime, args);

#   include "createMesh.H"

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Info<< nl << "Time: " << runTime.timeName() << endl;

        mesh.readUpdate();

        if(scalarBool)
        {
            Info<< nl << "Reading surface scalar field " << scalarFieldName << endl;

            surfaceScalarField surfScaField
            (
                IOobject
                (
                    scalarFieldName,
                    runTime.timeName(),
                    mesh,
                    IOobject::MUST_READ,
                    IOobject::NO_WRITE
                ),
                mesh
            );

            volVectorField volVecField
            (
                IOobject
                (
                    volFieldName,
                    runTime.timeName(),
                    mesh,
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                fvc::reconstruct(surfScaField)
            );

            Info<< nl << "Write volume vector field " << volFieldName << endl;

            volVecField.write();
        }
        else if(vectorBool)
        {
            Info<< nl << "Reading field " << vectorFieldName << endl;

            surfaceVectorField surfVecField
            (
                IOobject
                (
                    vectorFieldName,
                    runTime.timeName(),
                    mesh,
                    IOobject::MUST_READ,
                    IOobject::NO_WRITE
                ),
                mesh
            );

            volTensorField volTenField
            (
                IOobject
                (
                    volFieldName,
                    runTime.timeName(),
                    mesh,
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                fvc::reconstruct(surfVecField)
            );

            Info<< nl << "Write volume tensor field " << volFieldName << endl;

            volTenField.write();
        }
    }

    Info<< "\nEnd\n" << endl;

    return 0;
}


// ************************************************************************* //
