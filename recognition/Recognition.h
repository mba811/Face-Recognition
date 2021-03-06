/*
 * File:   Recognition.cpp
 * Author: Octavian Sima 
 *
 * Recognition module - recognize persons provided by detection module
 * The recognition process uses a subjects database - each person should have
 * a different folder (unique folder name - ID) with  ~10 face pictures
 * in different positions (frontal, profile (~45-60 degrees).
 * After recognition, information about corresponding person is extracted from
 * another database.
 */

#ifndef _RECOGNITION_H
#define	_RECOGNITION_H

#define EUCLIDIAN_DISTANCE      0
#define MAHALANOBIS_DISTANCE    1

#define FILE_SRC        0
#define DATABASE_SRC    1

#define DEFAULT_TRAINING_DATA_FILE  "trainingData.xml"
#define AVERAGE_IMAGE_FILE          "outAverageImage.pgm"
#define EIGENFACES_IMAGE_FILE       "outEigenfacesImage.pgm"

#include <opencv/cxcore.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>

#include "IRecognition.h"

class Recognition : implements IRecognition {
    
// Construction & Destruction
public:
    Recognition();
  
   ~Recognition();


//IRecognition implementation
public:

    /* Sets recognition type and parameters
     * recognitionThreshold - the minimum accepted percent for a person
     *          to be considered recognized
     * distanceType - type of distance used in closest face computation
     *     values: EUCLIDIAN_DISTANCE
     *             MAHALANOBIS_DISTANCE (provides better results)
     * faceImageExtension - image file type (pgm, jpg, etc.)
     * defaultTrainingDataFile - default xml file with training data
     */
    int initialize(float recognitionThreshold, int distanceType,
            char* faceImageExtension, char* defaultTrainingDataFile);

    /*
     * Performs training phase - learn faces
     * trainingSetSourceType - training face images source type
     *    values: FILE_SRC  - face images from a file
     *                      - each line contains: subjectID faceImage_Path
     *            DATABASE_SRC - all faces from a given database
     * pathToTrainingSet - path to File or to Database
     * saveEigenFaces - if true, save eigenfaces and average image to out files
     */
    int train(int trainingSetSourceType, char* pathToTrainingSet, bool saveEigenFaces);

    /*
     * Saves data from trainng phase into a given xml file
     * if fileName = NULL -> defaultTrainingDataFile is used
     */
    int saveTrainingData(char* fileName);

    /*
     * Loads training data from a given xml file
     * if fileName = NULL -> defaultTrainingDataFile is used
     */
    int loadTrainingData(char* fileName);

    /* 
     * Returns a vector of structs with recognized person IDs and
     * recognition confidence for the given faces
     *  faces - vector of faces to recognize
     *  resultsNo - number of best results for each face
     *            - if best result for a face is under recognitionThreshold,
     *              resultsNo will be 1 and personID will be 0
     * Obs: face images must have the same dimension with database images
     */
    vector<RecognitionResult> recognizeFaces(vector<IplImage*> faces, int resultsNo);

    /*
     * Function used for testing recognition performance
     *  facesFilePath - path to file that contains on each line info like:
     *      faceImagePath  personRealID
     * Recognition results for each face from file will be shown at system out
     * Obs1: testing face images must have the same dimension with database images
     * Obs2: you should first call train() or loadTrainingData() functions before
     * calling this
     */
    void testRecognitionPerformance(char* facesFilePath);
   
//functions used by recognition module
private:

    /*
     * Loads training face images from a file
     * Each line contains info like :
     *      subjectID faceImage_Path
     */
    int loadTrainingFaceImages(char* fileName);

    /*
     * Loads all faces for training phase from a given database
     * Faces are initialy stored into an array for future processing
     */
    int loadTrainingFacesDatabase(char* databasePath);

    /*
     * Applys PCA for dimensionality reduction
     * All training faces can be represented as a combination of average face
     * and eigen vectors with coresponding eigenvalues
     */
    int performPrincipalComponentAnalysis();

    /*
     * Projects training face images onto PCA subspace
     * using cvEigenDecomposite function
     */
    int projectTrainingImages();

    /*
     * Finds the most likely training face images for the test image.
     * Returns a structure with best recognitionConfidence values and
     * closest persons IDs
     *  resultsNo - number of best results for each face
     *            - if best result for a face is under recognitionThreshold,
     *              resultsNo will be 1 and personID will be 0
     */
    RecognitionResult findClosestFaces(float* projectedTestFace, int resultsNo);

    /*
     * Saves eigenfaces and average image after training to image files
     */
    void saveEigenFaces();

    /*
     * Get an 8-bit equivalent of the 32-bit float image.
     * Used in saveEigenFaces() function
     */
    IplImage* convertFloatImageToUcharImage(const IplImage *srcImg);


private:
    int trainFacesNo_;      // the number faces used for training
    int eigenVectorsNo_;    // the number of eigenvalues
    int distanceType_;      // distance between neigbours type
    float recogThreshold_;  // minimum accepted confidence for a person to be considered recognized
    char imageExtension[4]; // face images extension (jpg, bmp, etc)
    char* defaultTrainingDataFile_; //default xml file where training data is stored

    IplImage** faceImages_;     // array of face images
    IplImage** eigenVectors_;   // eigenvectors
    IplImage* averagedImage_;   // the averaged image

    CvMat* personRealIDs_;      // the real IDs of persons usead in trainig phase
    CvMat* eigenValues_;        // eigenvalues
    CvMat* projectedTrainFaces_;// projected training faces

};

#endif	/* _RECOGNITION_H */
