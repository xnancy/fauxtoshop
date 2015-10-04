/* Fauxtoshop.cpp
 * This source file performs a variety of photoshop shop effects on an image
 * Available filters: Scatter, Edge Detection, Green Screen, Comparison
 * The user is prompted for input of:
 *      1. image file name
 *      2. filter type
 *      3. file save location
 * as well as filter specific specifications.
 * Returns filted image saved to file
 *
 * Edited by: Nancy (Congwen) Xu
 * Date: 10/2/2015
 */

/* References
 * Class links consulted:
 * http://stanford.edu/~stepp/cppdoc/random.html
 * http://web.stanford.edu/class/archive/cs/cs106b/cs106b.1126/materials/cppdoc/Grid-class.html
 * http://stanford.edu/~stepp/cppdoc/strlib.html
 *
 * Outside help:
 * I used http://stackoverflow.com/questions/30250934/how-to-end-c-code to find out how to terminate a function
 *
*/

#include <iostream>
#include "console.h"
#include "gwindow.h"
#include "grid.h"
#include "simpio.h"
#include "strlib.h"
#include "gbufferedimage.h"
#include "gevents.h"
#include "random.h"

using namespace std;

static const int    WHITE = 0xFFFFFF;
static const int    BLACK = 0x000000;
static const int    GREEN = 0x00FF00;

static void         loadOriginalImage(GBufferedImage &img);
static void         getFilterNumber(string &filterNumber);
static bool         testFilterNumber(const string &filterNumber);
static void         applyFilter(GBufferedImage &img, const string &filterNumber);
static Grid<int>    applyScatter(const Grid<int> &grid);
static Grid<int>    applyEdgeDetection(const Grid<int> &originalGrid);
static Grid<int>    applyGreenScreen(const Grid<int> &originalGrid);
static void         applyComparison(const GBufferedImage &originalImage);
static void         getStickerLocation(int &placeRow, int &placeColumn);
static int          getColorDifference(const int &pix1, const int &pix2);
static void         saveFilteredImage(GBufferedImage &img);

static bool         openImageFromFilename(GBufferedImage &img, const string &filename);
static bool         saveImageToFilename(const GBufferedImage &img, const string &filename);
static void         getMouseClickLocation(int &row, int &col);

/* The main function is broken down into four main components
 *      1. Loading/displaying the original image (loadOriginalImage(img))
 *      2. Prompting for the filter effect (getFilterNumber(filterNumber))
 *      3. Applying the filter (applyFilter(img, filterNumber))
 *      4. Displaying/saving the filtered image (saveFilteredImage(img))
 *
 * Each component is treated as a separate function in the source file
 */
int main() {
    // Creates GWindow to display images
    GWindow gw;
    gw.setTitle("Fauxtoshop");
    gw.setVisible(true);

    cout << "Welcome to Fauxtoshop!" << endl;

    // Prompts for file name
    // Loads and displays file as GBufferedImage
    GBufferedImage img;
    loadOriginalImage(img);
    gw.setSize(img.getWidth(), img.getHeight());
    gw.add(&img, 0, 0);

    // Gets filter effect to be applied
    string filterNumber;
    getFilterNumber(filterNumber);

    // Applies filter effect to img
    applyFilter(img, filterNumber);

    // Displays and saves filtered image to file
    // Prompts for file save location
    gw.add(&img, 0, 0);
    saveFilteredImage(img);

    return 0;
}

/*
 * Loads prompted 'fileName' to GBufferedImage img
 *
 * Prints console greeting and prompts user for file image name
 * If prompted name is valid: loads image
 * If prompted name is blank: quits
 * Else: reprompts for image name
 */
static void loadOriginalImage (GBufferedImage &img) {
    string fileName;
    bool fileSuccess = false;
    while (!fileSuccess) {
        fileName = getLine("Enter name of image file to open (or blank to quit): ");
        if (fileName.compare("") == 0)
        {
            exit(0);
        }
        fileSuccess = openImageFromFilename(img, fileName);
    }
}

/*
 * Gets 'filterNumber' from user prompt
 *
 * Prints filter options and numbers
 * If prompted number is valid: sets filterNumber to given number
 * If prompted number is blank: quits
 * Else: reprompts for filter number
 */
static void getFilterNumber (string &filterNumber) {
    bool filterSuccess = false;
    while (!filterSuccess) {
        filterNumber = getLine(
                            "Which image filter would you like to apply? \n "
                            "\t 1 - Scatter \n"
                            "\t 2 - Edge Detection \n"
                            "\t 3 - \"Green Screen\" with another image \n"
                            "\t 4 - Compare image with another image \n"
                            "Your choice: "
                        );
        if (filterNumber.compare("") == 0)
        {
            exit(0);
        }
        filterSuccess = testFilterNumber(filterNumber);
    }
}

/*
 * This function checks if the 'filterNumber' is a valid filter number
 *
 * Returns:
 *      - true when the filter number is a valid filter option
 *      - false otherwise
 */

 static bool testFilterNumber(const string &filterNumber)
 {
     if (filterNumber.compare("1") == 0)
     {
         return true;
     }
     else if (filterNumber.compare("2") == 0)
     {
         return true;
     }
     else if (filterNumber.compare("3") == 0)
     {
         return true;
     }
     else if (filterNumber.compare("4") == 0)
     {
         return true;
     }
     else
     {
         return false;
     }
 }

 /*
  * This function applies the selected 'filterNumber' to the original image
  *
  * It replaces the original GBufferedImage with the new filtered GBufferedImage
  */

 static void applyFilter(GBufferedImage &img, const string &filterNumber)
 {
    Grid<int> originalGrid = img.toGrid();
    Grid<int> filteredGrid;
    if (filterNumber.compare("1") == 0)
    {
        filteredGrid = applyScatter(originalGrid);
        img.fromGrid(filteredGrid);
    }
    if (filterNumber.compare("2") == 0)
    {
        filteredGrid = applyEdgeDetection(originalGrid);
        img.fromGrid(filteredGrid);
    }
    if (filterNumber.compare("3") == 0)
    {
        filteredGrid = applyGreenScreen(originalGrid);
        img.fromGrid(filteredGrid);
    }
    if (filterNumber.compare("4") == 0)
    {
        applyComparison(img);
    }
  }

/*
* This function applies the scatter effect to a Grid<int>
*
* Returns Grid<int> of scattered image
*/
static Grid<int> applyScatter(const Grid<int> &originalGrid)
{
   int radius = -1;
   int numRows = originalGrid.numRows();
   int numColumns = originalGrid.numCols();

   while (!(1 <= radius && radius <= 100))
   {
       radius = getInteger("Enter degree of scatter [1-100]: ");
   }
   Grid<int> scatteredGrid(numRows, numColumns);

   for (int i = 0; i < numRows; i++)
   {
       for (int j = 0; j < numColumns; j++)
       {
           int scatterRow = -1;
           int scatterColumn = -1;
           while (scatterRow < 0 || scatterRow >= numRows || scatterColumn < 0 || scatterColumn >= numColumns )
           {
                scatterRow = randomInteger(i - radius, i + radius);
                scatterColumn = randomInteger(j - radius, j + radius);
           }
           scatteredGrid[i][j] = originalGrid[scatterRow][scatterColumn];
       }
   }
   return scatteredGrid;
}

/*
* This function applies the edge detection effect to Grid<int>
*
* Prompts user for edge detection threshold as int
* Returns new Grid<int> of edged image
*/
static Grid<int> applyEdgeDetection(const Grid<int> &originalGrid)
{
    int threshold = -1;
    int numRows = originalGrid.numRows();
    int numColumns = originalGrid.numCols();

    while (!(threshold > 0))
    {
        threshold = getInteger("Enter threshold for edge detection threshold: ");
    }

    Grid<int> edgedGrid(numRows, numColumns);
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numColumns; j++)
        {
            if (i > 0 && j > 0 && getColorDifference(originalGrid[i - 1][j - 1], originalGrid[i][j]) > threshold)
            {
                edgedGrid[i][j] = BLACK;
            }
            else if (i > 0 && getColorDifference(originalGrid[i - 1][j], originalGrid[i][j]) > threshold)
            {
                edgedGrid[i][j] = BLACK;
            }
            else if (i > 0 && j < numColumns - 1 && getColorDifference(originalGrid[i - 1][j + 1], originalGrid[i][j]) > threshold)
            {
                edgedGrid[i][j] = BLACK;
            }
            else if (j > 0 && getColorDifference(originalGrid[i][j - 1], originalGrid[i][j]) > threshold)
            {
                edgedGrid[i][j] = BLACK;
            }
            else if (i > 0 && j < numColumns - 1 && getColorDifference(originalGrid[i][j + 1], originalGrid[i][j]) > threshold)
            {
                edgedGrid[i][j] = BLACK;
            }
            else if (i < numRows - 1 && j > 0 && getColorDifference(originalGrid[i + 1][j - 1], originalGrid[i][j]) > threshold)
            {
                edgedGrid[i][j] = BLACK;
            }
            else if (i < numRows - 1 && getColorDifference(originalGrid[i + 1][j], originalGrid[i][j]) > threshold)
            {
                edgedGrid[i][j] = BLACK;
            }
            else if (i < numRows - 1 && j < numColumns - 1 && getColorDifference(originalGrid[i + 1][j + 1], originalGrid[i][j]) > threshold)
            {
                edgedGrid[i][j] = BLACK;
            }
            else
            {
                edgedGrid[i][j] = WHITE;
            }
        }
    }

    return edgedGrid;
}

/*
 * This function applies the green screen effect to Grid<int>
 *
 * Returns new Grid<int> of green screen image
 * Prompts user for sticker image name, tolerance, and overlay location
 * Applies mouse click for location if prompted location is blank
 */
 static Grid<int> applyGreenScreen(const Grid<int> &originalGrid)
 {
     int numRows = originalGrid.numRows();
     int numColumns = originalGrid.numCols();
     Grid<int> screenedGrid(numRows, numColumns);

     // Gets sticker grid from prompted image name
     string stickerName;
     GBufferedImage sticker;
     bool stickerSuccess = false;
     cout << "Now choose another another file to add to your background image." << endl;
     while (!stickerSuccess) {
         stickerName = getLine("Enter name of sticker image file to open: ");
         stickerSuccess = openImageFromFilename(sticker, stickerName);
     }
     Grid<int> stickerGrid = sticker.toGrid();

     // Gets tolerance value from user prompt
     int tolerance = -1;
     while (!(tolerance > 0))
     {
         tolerance = getInteger("Please enter a green screen tolerance: ");
     }

     //Gets sticker overlay placement location
     int placeRow = -1;
     int placeCol = -1;
     getStickerLocation(placeRow, placeCol);

     // Loops through original grid and populates filteredGrid
     // Checks for pixels with green color difference greater than the tolerance in sticker overlay
     // For high-diff pixels, uses sticker overlay pixel instead of original pixel
     int stickerNumRows = stickerGrid.numRows();
     int stickerNumCols = stickerGrid.numCols();

     for (int i = 0; i < numRows; i++)
     {
         for (int j = 0; j < numColumns; j++)
         {
            if ((placeRow <= i) && (i < (placeRow + stickerNumRows)) && (placeCol <= j) && (j < (placeCol + stickerNumCols)))
            {
                if (getColorDifference(stickerGrid[i - placeRow][j - placeCol], GREEN) > tolerance)
                {
                    screenedGrid[i][j] = stickerGrid[i - placeRow][j - placeCol], GREEN;
                }
                else
                {
                    screenedGrid[i][j] = originalGrid[i][j];
                }
            }
            else
            {
                screenedGrid[i][j] = originalGrid[i][j];
            }
         }
     }
     return screenedGrid;
 }

 /* This function performs a comparison of the original image with another image
  *
  * Prompts user for the name of the comparison image
  * Prints the number of pixels the two images differ by.
  */
 static void applyComparison (const GBufferedImage &originalImage)
 {
     // Prompt for compare image
     string compareFileName;
     bool compareFileSuccess = false;
     GBufferedImage compareImage;
     while (!compareFileSuccess) {
         compareFileName = getLine("Enter name of image file to compare with: ");
         compareFileSuccess = openImageFromFilename(compareImage, compareFileName);
     }

     int diff = originalImage.countDiffPixels(compareImage);
     string differ = "These images differ in ";
     string same = "These images are the same!";
     if (diff == 0)
     {
         cout << same << endl;
     }
     else
     {
         cout << (differ + integerToString(diff) + " pixel locations!") << endl;
     }
 }

/*
* This function prompts the user for a sticker overlay position
*
* It takes input in the form (row,col) and reprompts if format is wrong
* If the input is blank, the function allows the user to specify a location using mouse click
* Updates references placeRow and placeCol
*/

static void getStickerLocation(int &placeRow, int &placeCol)
{
   int commaIndex = -1;
   string userInput = "";
   string row;
   string column;
   while (!(commaIndex > 0 && startsWith(userInput, "(") && endsWith(userInput, ")") && placeRow >= 0 && placeCol >= 0))
   {
       userInput = getLine("Enter location to place image as \"\(row,col\)\" or blank to use mouse: ");
       commaIndex = stringIndexOf(userInput , ",");
       row = userInput.substr(1, commaIndex - 1);
       column = userInput.substr(commaIndex + 1, userInput.length() - commaIndex - 2);

       if (userInput.compare("") == 0)
       {
           cout << "Now click the background image to place new image: " << endl;
           getMouseClickLocation(placeRow, placeCol);
           string temp = "You chose (";
           cout << temp + integerToString(placeRow) + "," + integerToString(placeCol) + ")" << endl;
           break;
       }
       else if (commaIndex > 0 && stringIsInteger(row) && stringIsInteger(column))
       {
           placeRow = stringToInteger(row);
           placeCol = stringToInteger(column);
       }
   }
}

/*
 *
* This function returns the RGB color "difference" of two pixels
*
* It takes the two pixel values as ints
* Returns int difference as:
*       - max() of the absolute differences of R, G, B
*/
static int getColorDifference(const int &pix1, const int &pix2)
{
   int red1, red2, green1, green2, blue1, blue2;
   GBufferedImage::getRedGreenBlue(pix1, red1, green1, blue1);
   GBufferedImage::getRedGreenBlue(pix2, red2, green2, blue2);
   return max(max(abs(red1 - red2), abs(green1 - green2)), abs(blue1 - blue2));
}

/*
* Saves filtered image to prompted file name
*
* If prompted file name is valid (saveImageToFilename returns true): sets filterNumber to given number
* Otherwise, reprompts for file name
*/
static void saveFilteredImage(GBufferedImage &img)
{
    bool saveSuccess = false;
    string fileName;
    while (!saveSuccess)
    {
        fileName = getLine("Enter filename to save image (or blank to skip saving) : " );
        saveSuccess = saveImageToFilename(img, fileName);
    }
}

/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Attempts to open the image file 'filename'.
 *
 * This function returns true when the image file was successfully
 * opened and the 'img' object now contains that image, otherwise it
 * returns false.
 */
static bool openImageFromFilename(GBufferedImage &img, const string &filename) {
    try { img.load(filename); }
    catch (...) { return false; }
    return true;
}

/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Attempts to save the image file to 'filename'.
 *
 * This function returns true when the image was successfully saved
 * to the file specified, otherwise it returns false.
 */
static bool saveImageToFilename(const GBufferedImage &img, const string &filename) {
    try {
        img.save(filename);
    } catch (...) {
        return false;
    }
    return true;
}

/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Waits for a mouse click in the GWindow and reports click location.
 *
 * When this function returns, row and col are set to the row and
 * column where a mouse click was detected.
 */
static void getMouseClickLocation(int &row, int &col) {
    GMouseEvent me;
    do {
        me = getNextEvent(MOUSE_EVENT);
    } while (me.getEventType() != MOUSE_CLICKED);
    row = me.getY();
    col = me.getX();
}
