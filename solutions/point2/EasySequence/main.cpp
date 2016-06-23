/**
Software written by Juan Sebastian Lopez Villa, Software Developer.
Exclusive design for EasySolutions recruitment process. Jun/22/2016
*/

// Ui includes
#include "mainwindow.h"

// Qt includes
#include <QApplication>
#include <QRegularExpression>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QProgressDialog>

// C/C++ standard includes
#include <deque>
#include <cstdlib>
#include <time.h>
#include "frame.h"

using namespace std;

/** Incomming bytes emulation buffer */
std::deque<char> incommingBuffer;

void simIncommingBytes();

// Function for reading sequence from user.
QString readSequenceText(MainWindow &w) {
    bool ok = false;
    QString input;

    input = QInputDialog::getText(w.centralWidget(),
        "EasySequence - Sequence Finder",
        "Please enter the sequence you want to look for (0-4 only):", QLineEdit::Normal,
        "", &ok);

    return input;
}

// Function for filtering unallowed chars in text sequence provided by user
QString filterSequence(QString inputText) {
    QString sequence;
    QRegularExpression re("([01234])");
    QRegularExpressionMatchIterator i = re.globalMatch(inputText);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString digit = match.captured(1);
        sequence += digit.at(0);
    }

    return sequence;
}

// Function for asking user if searching more sequences
bool askDoItAgain(MainWindow &w) {
    // Store user response about do it again
    QMessageBox::StandardButton reply;

    reply = QMessageBox::question(w.centralWidget(), "Do it again?", "Do you want to find another sequence?", QMessageBox::Yes|QMessageBox::No);

    if(reply == QMessageBox::No) {
        return false;
    } else {
        return true;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);    
    MainWindow w;
    w.setWindowTitle("EasySequence");
    w.show();

    // Flag to find a new sequence. Defaults true.
    bool doItAgain = true;

    while(doItAgain) {
        // Read sequence from user
        QString input = readSequenceText(w);

        // Filter and store sequence
        QString sequence = filterSequence(input);

        // Seed rand number generator
        srand(time(NULL));

        // Frame container
        Frame frame(1, 0);  // We are going to process one-byte frames and 0 extra byte

        // Counter for valid frames
        int validFramesFound = 0;

        // Save sequence size
        int sequenceSize = sequence.size();

        // Validate sequence size
        if(sequenceSize == 0) {
            if(askDoItAgain(w)) {
                continue;
            } else {
                break;
            }
        }

        // Counter for sequence matches
        int sequenceCounter = 0;

        // Incomming bytes counter
        int bytesCounter = 0;

        // Flag for stopping incommingBytes loop
        bool sequenceFound = false;

        // Progress bar for keep application responsive for user
        QProgressBar * progressBar = new QProgressBar();
        progressBar->setRange(0, 0);
        progressBar->setTextVisible(false);

        QProgressDialog progress("Searching your sequence, please wait...", "Cancel search", 0, 0, NULL);
        progress.setBar(progressBar);
        progress.setWindowModality(Qt::WindowModal);
        progress.open();

        while (!sequenceFound) {
            // Simulate incomming bytes
            simIncommingBytes();

            // Process all incoming bytes one by one
           while(incommingBuffer.size()) {               
               // Read front element. incommingBuffer should be used as a FIFO queue.
               char byte = incommingBuffer.front();
			   
			   // After processing the byte, remove it from incommingBuffer.
               incommingBuffer.pop_front();

               // Count byte
               bytesCounter++;

               // Check if frame is complete
               if(frame.addByte(byte)) {                   
                    // Increase valida frames counter
                    validFramesFound++;

                    // Read frame payload
                    char payload = frame[0];

                    // Read next char from sequence
                    char sequenceChar = sequence.at(sequenceCounter).toLatin1() - '0';

                    // Check if payload is the next byte in sequence
                    if(payload == sequenceChar) {
                        // Increase sequence bytes counter
                        sequenceCounter++;

                        if(sequenceCounter == sequenceSize) {
                            // Sequence found
                            sequenceFound = true;

                            // Hide progress bar
                            progress.hide();

                            // Show message to user
                            QMessageBox::information(w.centralWidget(),
                                                    "Sequence Found",
                                                    "Congratulations, Sequence "+ sequence +" found after "+ QString::number(bytesCounter) +" bytes and "+ QString::number(validFramesFound) +" valid frames.", QMessageBox::Ok);

                            // Ask if search another sequence
                            doItAgain = askDoItAgain(w);

                            // Break while loop
                            break;
                        }
                    } else {
                        // Reset sequence search
                        sequenceCounter = 0;
                    }

                    // Reset frame container
                    frame.clear();
               }              

               progressBar->setValue(0);
               progress.setValue(0);
               QApplication::processEvents();
           }

           // Check if progress bar was cancelled
           if (progress.wasCanceled()) {
                doItAgain = askDoItAgain(w);
                break;
           }
       }       
    }

    // Say good bye to user.
    QMessageBox::information(w.centralWidget(),
                            "Good bye!",
                            "Have a nice day!", QMessageBox::Ok);

    // Quit Qt Application
    QCoreApplication::quit();

    return 0;
}

/**
 * Simulates incomming bytes. Generates pseudorandom byte sequences and frames
 * and puts it into incommingBuffer.
 * This function could be optimized but such effort is not required for this test.
 */
void simIncommingBytes()
{
   int i = rand()%10;
   if ( i < 5 ) {
      string data;

      data += static_cast<char>(i);
      data = Frame::buildFrame(data);
      incommingBuffer.insert( incommingBuffer.end(), data.begin(), data.end() );
   } else {
      int rndDtaCnt = rand()%10;
      for ( i = 0; i<rndDtaCnt; ++i ) {
            char to_insert = static_cast<char>(rand()%10);
            incommingBuffer.push_back(to_insert);
      }
   }
}
