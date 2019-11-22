#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphwindow.h"

#include "PageReplacement.h"


/*!
 * \brief MainWindow::MainWindow
 * Construct the Main (and only) Window for the program
 * \param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Set up the UI
    ui->setupUi(this);

    // Set up the title and icon
    this->setWindowTitle(QString::fromStdString("Page Replacement Algorithm"));


    // Connect the ref string generation button to the gen ref string function. This is conceptually
    // the same as a callback. Do the same for calc page faults button and function.
    connect(ui->btnGenerateReferenceString, SIGNAL (clicked()), this, SLOT (GenerateReferenceString()));
    connect(ui->btnCalculatePageFaults, SIGNAL (clicked()), this, SLOT (CalculatePageFaults()));

    // Set the ranges for the frames and pages
    ui->spinNumFrames->setRange(1, 7);
    ui->spinNumPages->setRange(0, 9);

    // Set the default values at the maximal values
    ui->spinNumFrames->setValue(7);
    ui->spinNumPages->setValue(9);

    // Set the value of the reference string to the one detailed in the requirements document
    ui->txtReferenceString->setText(QString::fromStdString("1, 2, 3, 4, 2, 1, 5, 6, 2, 1, 2, 3, 7, 6, 3, 2, 1, 2, 3, 6"));

    // Populate the combo box with the default vars for the algorithsm to be used
    ui->cmboAlgorithm->addItems(QStringList{"FIFO", "LRU", "OPT"});
}

/*!
 * \brief MainWindow::GenerateReferenceString
 *
 * SLOT for the Generate Ref String button that generates a random, complient
 * reference string of length 20, turns it into a string, and then puts it into the
 * text box.
 */
void MainWindow::GenerateReferenceString()
{
    // Generate the reference string, the size is defined as 20 in the requirement document
    // but the upper bound will be the number of frames in virtual memory
    std::vector<int> gen_ref_string = AbstractPageReplacement::GenerateRefString(20, ui->spinNumPages->value());

    // Turn it into a nice cstring with ", " delimiter
    std::stringstream ss;
    std::copy(gen_ref_string.begin(), gen_ref_string.end(), std::ostream_iterator<int>(ss, ", "));
    std::string s = ss.str();

    // Remove the trailing ", "
    s = s.substr(0, s.length() - 2);

    // Set the text box to the string. We aren't keeping track of this in the class which
    // might be a little naive but the user may modify it in the GUI and this way we don't have
    // to muck about with updating it
    ui->txtReferenceString->setText(QString::fromStdString(s));
}

/*!
 * \brief MainWindow::CalculatePageFaults is a slot for the calculate page faults
 * button. This function gets all the information from the GUI and calculates the
 * page faults using the user-selected algorithm.
 */
void MainWindow::CalculatePageFaults()
{
    // Get the reference string and ensure that it is valid.
    // Turn the string from the text box into a stream so we can work with it
    std::stringstream ss( ui->txtReferenceString->text().toStdString() );

    // Instantiate the ref string vector
    std::vector<int> ref_string;

    // Push every single digit number into the ref_string array
    // ignoring any chars that arent single digits. Because the
    // requirement document bound the page numbers between 0 and
    // 9 we can get away with doing it this way.
    for (char c; ss >> c;)
    {
        // If the current char is not a digit then we dont care
        if (!std::isdigit(c))
        {
            continue;
        }

        // If it is a char then sub '0' from it to get its' int value
        ref_string.push_back(c - '0');
    }


    int num_pages = ui->spinNumPages->value();
    int num_frames = ui->spinNumFrames->value();
    AbstractPageReplacement* PageReplacement;

    switch(ui->cmboAlgorithm->currentIndex())
    {
        case 0:
            PageReplacement = new FIFOPageReplacement(ref_string, num_pages, num_frames);
            break;
        case 1:
            PageReplacement = new LRUPageReplacement(ref_string, num_pages, num_frames);
            break;
        case 2:
            PageReplacement = new OPTPageReplacement(ref_string, num_pages, num_frames);
            break;
        default:
            break;
    }

    int page_faults = PageReplacement->CalculatePageFaults();
    QString status = QString("This configuration will give %1 page fault(s)").arg(page_faults);
    QMessageBox::information(this, tr("Page Fault Calculation"), status);

}

MainWindow::~MainWindow()
{
    delete ui;
}
