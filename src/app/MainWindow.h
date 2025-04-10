#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QSignalMapper>
#include <QScrollArea>
#include <QIcon>
#include <QStatusBar>
#include "Renderer.h"
#include "ResizeDialog.h"
#include "ColorCountDialog.h"
#include "ColorPicker.h"

namespace Ui{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow();
    ~MainWindow() ;

public slots:
	void newFile();
	void openFile();
	void openFileImport();
	void openFileSmart();
	void resizeImage();
	void smartResizeImage();
	void saveFile();
	void closeProgram();
	void getColorCount();
	void setBrushColor();
	void setEraser();
	void setBackgroundColor();
	void setOutlineColor();
	void setColorPalette();
	void openPattern(QString dir);
	void repaintAll();
	void rotateTranspose();
	void rotateClockwise();
	void rotateCounterClockwise();
	void rotate180();
	void rotateFlipX();
	void rotateFlipY();
	void zoomIn();
	void zoomOut();
	void zoomNormal();
	void about();

	void closeEvent(QCloseEvent *);

private:
    Renderer *renderWidget;
    Ui::MainWindow *ui ;
	// Custom Color Menu
	ColorPicker *picker;

	// Initalizer Functions
    void createActions();
	/**
	 * @brief Recursively adds patterns to the given menu
	 * @param menu The menu to add to (patterns to start)
	 * @param dir The starter directory
	 */
	void findPatterns(QMenu *menu, QDir dir = QDir("./Patterns/."));
	/**
	 * @brief Opens settings.txt and sets the size of the window to that if it exists
	 * @return The size in the txt or 600,400
	 */
	QSize startupSize();
	/**
	 * @brief Updates the bar on the bottom of the screen
	 */
	void updateStatus();

	// Status
	QLabel *status;

	// Menus
	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *transformSubMenu;
	QMenu *zoomSubMenu;
	QMenu *colorMenu;
	QMenu *patternMenu;
	QMenu *aboutMenu;

	// Actions
	QAction *newAct;
	QAction *openAct;
	QAction *openImportAct;
	QAction *openSmartImportAct;
	QAction *resizeImageAct;
	QAction *saveAct;
	QAction *closeAct;
	QAction *repaintAllAct;
	QAction *rotateTransposeAct;
	QAction *rotateClockwiseAct;
	QAction *rotateCounterClockwiseAct;
	QAction *rotate180Act;
	QAction *rotateFlipXAct;
	QAction *rotateFlipYAct;
	QAction *zoomInAct;
	QAction *zoomOutAct;
	QAction *zoomNormalAct;
	QAction *getColorCountAct;
	QAction *setBrushColorAct;
	QAction *setEraserAct;
	QAction *setBackgroundColorAct;
	QAction *setOutlineColorAct;
	QAction *setColorPaletteAct;
	QAction *aboutAct;
	QSignalMapper *signalMapper;
};

#endif
