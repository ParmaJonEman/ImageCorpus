### Image Corpus
### Author: Jon Eman
### Date: 9/26/2022

### Build
##### To build, install OpenCV on your computer, change the OpenCV path in the CMAKE file, and use the standard CMAKE commands for building.

### Usage

##### Create an input folder with subdirectories named after the category of photos they contain.
##### Each photo should have a metadata file named the same way as the photo, but with an xml extension.
##### Appropriate structure:
    Images
    -cats
    --cat1.jpg
    --cat1.xml
    --cat2.jpg
    --cat2.xml
    -dogs
    --dog1.jpg
    --dog1.xml
    --dog2.jpg
    --dog2.xml
##### The metadata xml files should be in the format:

    <?xml version="1.0"?>
    <opencv_storage>
    <date_acquisition>"8/3/2018"</date_acquisition>
    <modality_acquisition>"Taken myself"</modality_acquisition>
    <copyright>"Jon Eman"</copyright>
    <annotation>"Orange cat in cat bed"</annotation>
    </opencv_storage>

##### Run the program with the following command structure:
    corpus <input directory> <output directory> --rows=<number of rows> --columns=<number of columns> -type=<output file type> [-g] [-a]

##### Where -g is the optional parameter for greyscale and -a is the optional parameter for maintaining aspect ratio.