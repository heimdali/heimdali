#include <iostream>

#include <tclap/CmdLine.h>

#include <itkVectorExpandImageFilter.h>
#include <itkExpandImageFilter.h>

#include "heimdali/itkhelper.hxx"
#include "heimdali/cmdreader.hxx"
#include "heimdali/cmdwriter.hxx"
#include "heimdali/cli.hxx"
#include "heimdali/error.hxx"
#include "heimdali/version.hxx"


// VectorExpandImageFilter work on Image of vector pixel, not on VectorImage

using namespace std;

int main(int argc, char** argv)
{ 
    try {

    vector<string> tclap_argv = Heimdali::preprocess_argv(argc, argv);

    TCLAP::CmdLine cmd("Image enlargement or reduction",' ', HEIMDALI_VERSION);

    // -x -y
    //TCLAP::ValueArg<int> xValue("x","rows", "New number of rows",false,0,"X",cmd);
    //TCLAP::ValueArg<int> yValue("y","columns", "New number of columns",false,0,"Y",cmd);

    // -sc
    TCLAP::ValueArg<float> scValue("","sc", "Enlargement factor",false,0,"X",cmd);

    // input.h5
    TCLAP::UnlabeledValueArg<string> inputFilenameArg("inputFilename", 
        "Input image file name.",true,"input.h5","IMAGE-IN");
    cmd.add(inputFilenameArg);

    // output.h5
    TCLAP::UnlabeledValueArg<string> outputFilenameArg("outputFilename", 
        "Output image file name.",true,"output.h5","IMAGE-OUT");
    cmd.add(outputFilenameArg);

    cmd.parse(tclap_argv);

    // Command line tool reader.
    typedef Heimdali::CmdReader<Heimdali::ImageFloat> ReaderType;
    ReaderType* cmdreader = ReaderType::make_cmd_reader(0, inputFilenameArg.getValue());

    // Expand filter
    typedef itk::ExpandImageFilter<Heimdali::ImageFloat, Heimdali::ImageFloat> ExpanderType;
    ExpanderType::Pointer expander = ExpanderType::New();
    expander->SetExpandFactors(scValue.getValue());

    // Command line tool writer.
    typedef Heimdali::CmdWriter<Heimdali::ImageFloat> WriterType;
    WriterType* cmdwriter = WriterType::make_cmd_writer(outputFilenameArg.getValue());

    size_t iregionmax = 1E+06;
    for (size_t iregion=0 ; iregion<iregionmax ; iregion++) {
        // Read input.
        cmdreader->next_iteration();
        if (cmdreader->is_complete()) break;

        expander->SetInput(cmdreader->GetOutput());
        cmdwriter->Write( expander->GetOutput() );
        cmdwriter->Update();
    }

    }

    // Command line parser.
    catch (TCLAP::ArgException &e) { 
        cerr << "izoom: ERROR: " << e.error() << " for arg " << e.argId() << endl;
    }


    // Input/output.
    catch (Heimdali::IOError &e) {
        cerr << "izoom: ERROR: " << e.getMessage() << endl;
    }
}
