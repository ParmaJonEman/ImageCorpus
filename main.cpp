// ImageBrowser.cpp:	This program takes a directory of images with identically named xml documents (containing meta data)
//                      and normalizes their sizes. Users have an option to maintain aspect ratio and convert to greyscale.
// Author:				Jon Eman
// Date:				9/26/2022


#include <direct.h>
#include "main.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
    String directory;
    int rows;
    int columns;
    int preserveAspectRatio = 0;
    int greyScale = 0;
    String outputType = "";
    String outdir = "";
    if (parseParameters(argc, argv, &directory, &rows, &columns, &preserveAspectRatio, &greyScale, &outputType, &outdir))
    {
        cout << "Selected resolution: " << columns << "x" << rows << "\n" << endl;
        cout << outputType << endl;
        cout << outdir << endl;
        _mkdir(&outdir[0]);
        vector<string> vectorOfFiles = list_directory(directory.c_str(), outdir);
        if (processImages(vectorOfFiles, rows, columns, preserveAspectRatio, greyScale, outputType, outdir, directory))
        {
            cerr << "A fatal error occurred, please review the error message, make changes, and try again" << endl;
            return(1);
        }
        cout << "\n" << "Thanks for using Image Corpus! " << endl;
    }
    return(0);
}

static int processImages(vector<string> vectorOfFiles, int rows, int columns, int preserveAspectRatio, int greyScale, String outputType, String outdir, String indir)
{
    ofstream fw(outdir + "\\metadeta.txt", std::ofstream::out);
    for (int i = 0; i < vectorOfFiles.size(); i++)
    {
        try
        {
            String imageName;
            String imageDirectory;
            String imageFileExtension;
            String imageNameNoExtension;
            String thisOutputType = outputType;
            Mat image;

            // Try to read the file
            if (greyScale){
                image = imread(vectorOfFiles[i], IMREAD_GRAYSCALE);
            }
            else{
                image = imread(vectorOfFiles[i]);
            }

            // If the file cannot be read, throw an exception which deletes the file from the vector and continues
            if (image.empty())
                throw (string("Cannot open input image ") + vectorOfFiles[i]);


            // Gather and print some image meta data
            imageName = vectorOfFiles[i].substr(vectorOfFiles[i].find_last_of("/\\") + 1);
            imageDirectory = vectorOfFiles[i].substr(0, vectorOfFiles[i].find_last_of("/\\"));
            String imagePath = vectorOfFiles[i].substr(0, vectorOfFiles[i].find_last_of("/\\") + 1) + imageName;
            imageNameNoExtension = imageName.substr(0, imageName.find_last_of(".\\"));
            String imageMetaPath = vectorOfFiles[i].substr(0, vectorOfFiles[i].find_last_of("/\\") + 1) + imageNameNoExtension + ".xml";
            if (thisOutputType == ""){
                thisOutputType = imageFileExtension;
            }

            // Scale image and print new scaled size
            Mat scaledImage = scaleImage(image, rows, columns, preserveAspectRatio);
            cout << "Scaled image size : " << scaledImage.cols << "x" << scaledImage.rows << "\n" << endl;

            imageDirectory.replace(0, indir.length(), outdir);

            // Save image
            imwrite(imageDirectory + "//" + imageNameNoExtension + "." + thisOutputType, scaledImage);

            //Open image meta data
            FileStorage fs;
            fs.open(imageMetaPath, FileStorage::READ);
            cout << imageMetaPath << endl;
            cout << (string) fs["date_acquisition"] << endl;
            cout << (string) fs["modality_acquisition"] << endl;
            cout << (string) fs["copyright"] << endl;
            cout << (string) fs["annotation"] << endl;

            //Write image meta data to flat file
            fw << "image: " << i << " " + imageDirectory + "/" + imageNameNoExtension + "." + thisOutputType;
            fw << " date_acquisiton: " + (string) fs["date_acquisition"];
            fw << " modality_acquisition: " + (string) fs["modality_acquisition"];
            fw << " copyright: " + (string) fs["copyright"];
            fw << " annotation: " + (string) fs["annotation"];
            fw << endl;
        }
        catch (string& str)
        {
            vectorOfFiles.erase(vectorOfFiles.begin() + i);
            i--;
        }
        catch (Exception& e)
        {
            // Alerts user of an exception that cannot be handled gracefully, and terminates program
            cerr << "Error: " << vectorOfFiles[i] << ": " << e.msg << endl;
            return (1);
        }
    }
    fw.close();
    return (0);
}

static Mat scaleImage(Mat sourceImage, int rows, int columns, int preserveAspectRatio)
{
    // First we get the ratio between the source rows and the goal rows, and then the same for the columns
    float rowRatio = static_cast<float>(rows) / static_cast<float>(sourceImage.rows);
    float colRatio = static_cast<float>(columns) / static_cast<float>(sourceImage.cols);

    // Then, if the user selected to preserve aspect ratio,
    // we pick the ratio that is the smallest, because we want to scale the image as little as possible
    // and we want the rows and columns to be scaled by the same amount
    if (preserveAspectRatio){
        float ratio = min(rowRatio, colRatio);
        rowRatio = ratio;
        colRatio = ratio;
        cout << "Preserving aspect ratio" << endl;
        cout << "Scale ratio is: " << ratio << endl;
    }

    // The first triangle of the affine transformation is (0,0), (last column, 0), (last row, 0)
    Point2f srcTri[3];
    srcTri[0] = Point2f(0.f, 0.f);
    srcTri[1] = Point2f(sourceImage.cols - 1.f, 0.f);
    srcTri[2] = Point2f(0.f, sourceImage.rows - 1.f);


    // The second triangle of the affine transformation is (0,0), (last column * ratio, 0), (last row * ratio, 0)
    Point2f dstTri[3];
    dstTri[0] = Point2f(0.f, 0.f);
    dstTri[1] = Point2f((sourceImage.cols * colRatio) - 1.f, 0.f);
    dstTri[2] = Point2f(0.f, (sourceImage.rows * rowRatio) - 1.f);

    // Create the transformation matrix and a blank destination matrix
    Mat warp_mat = getAffineTransform(srcTri, dstTri);
    Mat warp_dst = Mat::zeros((sourceImage.rows) * rowRatio, (sourceImage.cols) * colRatio, sourceImage.type());

    // Execute affine transformation
    warpAffine(sourceImage, warp_dst, warp_mat, warp_dst.size());
    return warp_dst;
}

static int parseParameters(int argc, char** argv, String* directory, int* rows, int* cols, int* preserveAspectRatio, int* greyScale, String* outputType, String* outdir)
{
    String keys =
            {
                    "{help h usage ? |                            | print this message   }"
                    "{@directory      | | directory you want to use	}"
                    "{rows           |480| number of rows	}"
                    "{columns        |640| number of columns	}"
                    "{a       || preserve aspect ratio	}"
                    "{g       || preserve aspect ratio	}"
                    "{type       || preserve aspect ratio	}"
                    "{@outdir       || preserve aspect ratio	}"
            };

    // Get required parameters. If any are left blank, defaults are set based on the above table
    // If no directory is passed in, or if the user passes in a help param, usage info is printed
    CommandLineParser parser(argc, argv, keys);
    parser.about("ImageBrowser v1.0");

    if (!parser.has("@directory") || parser.has("help"))
    {
        parser.printMessage();
        return(0);
    }

    if (parser.has("a"))
    {
        *preserveAspectRatio = 1;
    }

    if (parser.has("g"))
    {
        *greyScale = 1;
    }

    if (parser.has("@outdir"))
    {
        *outdir = parser.get<String>("@outdir");
    }
    else
    {
        *outdir = parser.get<String>("@directory") + ".corpus";
    }

    if (parser.has("type"))
    {
        if (parser.get<String>("type") == "jpg" || parser.get<String>("type") == "tif" || parser.get<String>("type") == "bmp" || parser.get<String>("type") == "png")
        {
            *outputType = parser.get<String>("type");
        }
        else
        {
            cout << "Invalid type specified" << endl;
            return(0);
        }

    }

    *directory = parser.get<String>("@directory");
    *rows = parser.get<int>("rows");
    *cols = parser.get<int>("columns");
    return(1);
}

static vector<string> list_directory(const char* dirname, String outdir)
{

    // Scan files in directory
    struct dirent** files;
    vector<string> vectorOfFiles;
    int n = scandir(dirname, &files, NULL, alphasort);

    // Empty folders return an empty vector. Application is not terminated, because this could be a child folder in a folder that does have images.
    if (n < 0)
    {
        fprintf(stderr,
                "Cannot open %s (%s)\n", dirname, strerror(errno));
        return vectorOfFiles;
    }

    // Loop through file names
    for (int i = 0; i < n; i++)
    {
        // Get pointer to file entry and create full file path
        struct dirent* ent = files[i];
        string dirnameRoot;
        if (dirname != nullptr)
        {
            dirnameRoot = dirname + string("/");
        }
        const string filePath = dirnameRoot + string(ent->d_name);

        switch (ent->d_type)
        {
            // If the entry is a file, push it onto the vectorOfFiles
            case DT_REG:
                vectorOfFiles.push_back(filePath);
                break;

                // If the entry is a directory, execute method recursively and insert results into vectorOfFiles
                // and we create the matching output directory while we're there
            case DT_DIR:
                if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                    String pathStr(filePath);
                    String srcDirStr(dirname);
                    pathStr.replace(0, srcDirStr.length(), outdir);
                    _mkdir(&pathStr[0]);
                    vector<string> vectorOfFilesChildDir = list_directory(filePath.c_str(), pathStr);
                    vectorOfFiles.insert(vectorOfFiles.end(), vectorOfFilesChildDir.begin(), vectorOfFilesChildDir.end());
                }
                break;

            default:
                cout << "Unsupported file or folder type";
        }

    }

    // Release file names
    for (int i = 0; i < n; i++)
    {
        free(files[i]);
    }
    free(files);

    return vectorOfFiles;
}
