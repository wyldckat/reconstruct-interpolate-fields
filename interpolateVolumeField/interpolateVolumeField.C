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
    interpolateVolumeField

Description
    Interpolate vol*Field to surface*Field.

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
        "Interpolate vol*Field to surface*Field."
    );
    argList::addOption
    (
        "scalar",
        "volumeScalarField",
        "the name of the volumeScalarField"
    );
    argList::addOption
    (
        "vector",
        "volumeVectorField",
        "the name of the volumeVectorField"
    );
    argList::validArgs.append
    (
      "outputSurface*Field"
    );
#   include "addRegionOption.H"

#   include "setRootCase.H"
#   include "createTime.H"

    const fileName surfFieldName = args[1];
    
    fileName scalarFieldName, vectorFieldName;
    bool scalarBool = args.optionReadIfPresent("scalar", scalarFieldName);
    bool vectorBool = args.optionReadIfPresent("vector", vectorFieldName);

    if(scalarBool && vectorBool)
    {
        FatalErrorIn(args.executable())
            << "Pick only one type of volume field, either scalar or vector, not both."
            << exit(FatalError);
    }
    else if(!scalarBool && !vectorBool)
    {
        FatalErrorIn(args.executable())
            << "Pick one type of volume field, -scalar or -vector."
            << exit(FatalError);
    }
    
    if(scalarBool)
    {
        if (scalarFieldName == surfFieldName)
        {
            FatalErrorIn(args.executable())
                << "Volume field cannot have the same name as the surface field."
                << exit(FatalError);
        }
    }
    else if(vectorBool)
    {
        if (vectorFieldName == surfFieldName)
        {
            FatalErrorIn(args.executable())
                << "Volume field cannot have the same name as the surface field."
                << exit(FatalError);
        }
    }
    
    instantList timeDirs = timeSelector::select0(runTime, args);

#   include "createNamedMesh.H"

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Info<< nl << "Time: " << runTime.timeName() << endl;

        mesh.readUpdate();

        if(scalarBool)
        {
            Info<< nl << "Reading volume scalar field " << scalarFieldName << endl;

            volScalarField volScaField
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

            surfaceScalarField surfScaField
            (
                IOobject
                (
                    surfFieldName,
                    runTime.timeName(),
                    mesh,
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                fvc::interpolate(volScaField)
            );

            Info<< nl << "Write surface scalar field " << surfFieldName << endl;

            surfScaField.write();
        }
        else if(vectorBool)
        {
            Info<< nl << "Reading volume field " << vectorFieldName << endl;

            volVectorField volVecField
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

            surfaceVectorField surfVectorField
            (
                IOobject
                (
                    surfFieldName,
                    runTime.timeName(),
                    mesh,
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                fvc::interpolate(volVecField)
            );

            Info<< nl << "Write surface vector field " << surfFieldName << endl;

            surfVectorField.write();
        }
    }

    Info<< "\nEnd\n" << endl;

    return 0;
}


// ************************************************************************* //
