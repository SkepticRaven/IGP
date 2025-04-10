#include <QtWidgets>

#include "MainWindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow() :
    ui(new Ui::MainWindow)
{
    //Ui file does it's own thing here
    ui->setupUi(this) ;

	renderWidget = new Renderer;
	renderWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    QWidget *widget = new QWidget;
    setCentralWidget(widget);

    ui->scrollArea->setWidget(renderWidget);

	// Create the custom color picker, connect to Renderer

	picker = new ColorPicker( this ) ;
	connect( picker, &ColorPicker::colorChanged, renderWidget, &Renderer::updateBrushColor ) ;
	connect( picker, &ColorPicker::swappedButton, renderWidget, &Renderer::swapBrush ) ;
	ui->layout->addWidget(picker) ;
    widget->setLayout(ui->layout);

	signalMapper = new QSignalMapper(this);
	createActions();

    connect(signalMapper, &QSignalMapper::mappedString, this, &MainWindow::openPattern);
    findPatterns(ui->menuPatterns);

    //Creates the status bar
	statusBar();
    status = new QLabel;
	updateStatus();
    statusBar()->addWidget(status);

    this->setWindowIcon(QIcon("./Resources/IGP.png"));
	this->resize(startupSize());

	//renderWidget->setPattern(QDir("./Patterns/Default"));
}

MainWindow::~MainWindow()
{
    delete ui ;
}

void MainWindow::newFile()
{
	ResizeDialog resizeDialog(1, 1, 1, 1, 2000, 2000, QString("New Image Size"));
	
	if (resizeDialog.exec() == QDialog::Accepted)
	{
		renderWidget->newImage(resizeDialog.getWidth(),resizeDialog.getHeight());
		renderWidget->resize(renderWidget->getMinSize());;
	}
	updateStatus();
}

void MainWindow::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "./Img/", tr("Image Files (*.png *.jpg *.bmp)"));
	if (!fileName.isEmpty())
		renderWidget->loadImage(fileName);

	renderWidget->resize(renderWidget->getMinSize());
	updateStatus();
}

void MainWindow::openFileImport()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "./Img/", tr("Image Files (*.png *.jpg *.bmp)"));
	if (!fileName.isEmpty())
	{
		renderWidget->loadImage(fileName, false);
		resizeImage();
	}

	renderWidget->resize(renderWidget->getMinSize());
	updateStatus();
}

void MainWindow::openFileSmart()
{
	if (renderWidget->hasPatternSelected())
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "./Img/", tr("Image Files (*.png *.jpg *.bmp)"));
		if (!fileName.isEmpty())
		{
			renderWidget->loadImage(fileName, false);
			smartResizeImage();
			renderWidget->resize(renderWidget->getMinSize());
		}
	}
	else
		QMessageBox::warning(this, tr("Warning"), tr("First select a pattern from the pattern menu to use this function."));
	updateStatus();
}

void MainWindow::resizeImage()
{
	QSize currentSize = renderWidget->getImageSize();
	ResizeDialog resizeDialog(currentSize.width(), currentSize.height(), 1, 1, 2000, 2000);
	
	if (resizeDialog.exec() == QDialog::Accepted)
	{
		renderWidget->resizeImage(resizeDialog.getWidth(),resizeDialog.getHeight());
		renderWidget->resize(renderWidget->getMinSize());;
	}
	updateStatus();
}

void MainWindow::smartResizeImage()
{
	QSize currentSize = renderWidget->smartResizeTip();
	ResizeDialog resizeDialog(currentSize.width(), currentSize.height(), 1, 1, 2000, 2000);

	if (resizeDialog.exec() == QDialog::Accepted)
	{
		renderWidget->smartResize(resizeDialog.getWidth(),resizeDialog.getHeight());
		renderWidget->resize(renderWidget->getMinSize());;
	}
	updateStatus();
}

void MainWindow::saveFile()
{
	if (renderWidget->hasImageSelected())
	{
		QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "./Img/", tr("Image Files (*.png *.jpg *.bmp)"));
		if (!fileName.isEmpty())
			renderWidget->saveImage(fileName);
	}
	else
	{
		QMessageBox::warning(this, tr("No Image to Save"), tr("You have no image to save."));
	}
}

void MainWindow::closeProgram()
{
	this->close();
}

void MainWindow::getColorCount()
{
	QVector<QRgb> *colors = new QVector<QRgb>;
	QVector<int> *counts = new QVector<int>;
	renderWidget->countColors(colors, counts);

	if (colors->count()>25)
	{
		int selection = QMessageBox::question(this, tr("Lots of Colors"), QString("There are "+QString::number(colors->count())+" different colors. Are you sure you wish to display every single one? Doing so may take awhile.\n\nIf you wish to reduce the number of colors, please use the \"Color\"->\"Palette Change\" function."), QMessageBox::Yes, QMessageBox::No);
		if (selection==QMessageBox::Yes)
		{
			ColorCountDialog dialog(colors,counts);
			dialog.exec();
		}
	}
	else
	{
		ColorCountDialog dialog(colors,counts);
		dialog.exec();
	}

	delete colors;
	delete counts;
}

void MainWindow::setBrushColor()
{
	QColor color = QColorDialog::getColor(Qt::white, this, tr("Select Brush Color"));
	if( color.isValid() )
		picker->overrideSelectedColor( color ) ;
}

void MainWindow::setEraser()
{
	renderWidget->setBrushColor(QColor(255,255,255,0));
}

void MainWindow::setBackgroundColor()
{
	QColor color = QColorDialog::getColor(Qt::white, this, tr("Select Background Color"));
	if (color.isValid())
		renderWidget->setBackgroundColor(color);
}

void MainWindow::setOutlineColor()
{
	QColor color = QColorDialog::getColor(Qt::black, this, tr("Select Background Color"));
	if (color.isValid())
		renderWidget->setOutlineColor(color);
}

void MainWindow::setColorPalette()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Palette"), "./Palettes/", tr("Image Files (*.png *.jpg *.bmp)"));
	if (!fileName.isEmpty())
		renderWidget->changePalette(fileName);

}

void MainWindow::openPattern(QString dir)
{
	if (renderWidget->setPattern(QDir(dir)))
		renderWidget->resize(renderWidget->getMinSize());
	else
	{
		QMessageBox::warning(this, tr("Pattern Load Failed"), tr("There was an error while loading the pattern. Please select a new pattern."));
		renderWidget->resize(renderWidget->getMinSize());
	}
	updateStatus();
}

void MainWindow::repaintAll()
{
	renderWidget->repaintImage();
}

void MainWindow::rotateTranspose()
{
	renderWidget->rotateTranspose();
	renderWidget->resize(renderWidget->getMinSize());
}

void MainWindow::rotateClockwise()
{
	renderWidget->rotateClockwise();
	renderWidget->resize(renderWidget->getMinSize());
	updateStatus();
}

void MainWindow::rotateCounterClockwise()
{
	renderWidget->rotateCounterClockwise();
	renderWidget->resize(renderWidget->getMinSize());
	updateStatus();
}

void MainWindow::rotate180()
{
	renderWidget->rotate180();
	renderWidget->resize(renderWidget->getMinSize());
}

void MainWindow::rotateFlipX()
{
	renderWidget->rotateFlipX();
	renderWidget->resize(renderWidget->getMinSize());
}

void MainWindow::rotateFlipY()
{
	renderWidget->rotateFlipY();
	renderWidget->resize(renderWidget->getMinSize());
}

void MainWindow::zoomIn()
{
	renderWidget->zoomIn();
	renderWidget->resize(renderWidget->getMinSize());
}

void MainWindow::zoomOut()
{
	renderWidget->zoomOut();
	renderWidget->resize(renderWidget->getMinSize());
}

void MainWindow::zoomNormal()
{
	renderWidget->zoomNormal();
	renderWidget->resize(renderWidget->getMinSize());
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("About IGP 3.0"), tr("The <b>Irregular Grid Painter</b> [IGP] allows the user to paint patterned tiles. This program's first version originated within the chainmaille community at M.A.I.L. by user Zlosk using the .Net framework. The second version, written by user aganim, used javascript and HTML5. This version is written by user TrenchCoatGuy [Brian Geuther] using the QT framework."));
}

void MainWindow::closeEvent(QCloseEvent *)
{
	// Save current window settings
	QFile closeFile("./Resources/Settings.txt");
	closeFile.open(QIODevice::WriteOnly|QIODevice::Text);
	QTextStream output(&closeFile);

	output << "WindowWidth=" << QString::number(this->size().width()) << "\nWindowHeight=" << QString::number(this->size().height()) << "\n";
	output.flush();

	// Prompt if image unsaved
	if (renderWidget->imageChanged())
	{
		int selection = QMessageBox::question(this, tr("File Unsaved"), tr("You have made changes to the currently opened image. Would you like to save your changes?"), QMessageBox::Yes, QMessageBox::No);
		if (selection==QMessageBox::Yes)
			saveFile();
	}
}

void MainWindow::createActions()
{
	// --- File Menubar ---
	// File Actions
    ui->actionNew->setShortcut(QKeySequence::New) ;
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::newFile) ;

    ui->actionOpen->setShortcut(QKeySequence::Open) ;
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::openFile) ;

    connect(ui->actionImport, &QAction::triggered, this, &MainWindow::openFileImport) ;
    connect(ui->actionSmart_Import, &QAction::triggered, this, &MainWindow::openFileSmart) ;
    connect(ui->actionResize, &QAction::triggered, this, &MainWindow::resizeImage) ;

    ui->actionSave->setShortcut(QKeySequence::Save) ;
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveFile) ;

    ui->actionClose->setShortcut(QKeySequence::Close) ;
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::closeProgram) ;

	// --- Edit Menubar ---
    ui->actionRepaint_All->setShortcut(QKeySequence::Refresh) ;
    connect(ui->actionRepaint_All, &QAction::triggered, this, &MainWindow::repaintAll) ;

	// Rotate Image Actions
    connect(ui->actionRotate_Transpose, &QAction::triggered, this, &MainWindow::rotateTranspose) ;
    connect(ui->actionRotate_Clockwise, &QAction::triggered, this, &MainWindow::rotateClockwise) ;
    connect(ui->actionRotate_Counterclockwise, &QAction::triggered, this, &MainWindow::rotateCounterClockwise) ;
    connect(ui->actionRotate_180, &QAction::triggered, this, &MainWindow::rotate180) ;
    connect(ui->actionFlipX, &QAction::triggered, this, &MainWindow::rotateFlipX) ;
    connect(ui->actionFlip_Y, &QAction::triggered, this, &MainWindow::rotateFlipY) ;

	// Zoom Actions
    ui->actionZoom_In->setShortcut( QKeySequence( "Ctrl+=" ) ) ;
    connect(ui->actionZoom_In, &QAction::triggered, this, &MainWindow::zoomIn) ;

    ui->actionZoom_Out->setShortcut(QKeySequence::ZoomOut) ;
    connect(ui->actionZoom_Out, &QAction::triggered, this, &MainWindow::zoomOut) ;

    connect(ui->actionReset_Zoom, &QAction::triggered, this, &MainWindow::zoomNormal) ;

	// --- Color Menubar ---
	// Color Actions

    connect(ui->actionColor_Count, &QAction::triggered, this, &MainWindow::getColorCount) ;
    connect(ui->actionBrush_Color, &QAction::triggered, this, &MainWindow::setBrushColor) ;
    connect(ui->actionEraser, &QAction::triggered, this, &MainWindow::setEraser) ;
    connect(ui->actionBackground_Color, &QAction::triggered, this, &MainWindow::setBackgroundColor) ;
    connect(ui->actionOutline_Color, &QAction::triggered, this, &MainWindow::setOutlineColor) ;
    connect(ui->actionColor_Pallette, &QAction::triggered, this, &MainWindow::setColorPalette) ;

	// --- About Menubar ---
	// About Action
    ui->actionAbout->setShortcut(QKeySequence::HelpContents) ;
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about) ;
}

void MainWindow::findPatterns(QMenu *menu, QDir dir)
{
	// Filter for subfolders
	QStringList filters;
	filters << "*";
	dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
	QStringList subDirs = dir.entryList(filters);

	while (subDirs.length())
	{
		QString folder = subDirs.takeFirst();		
		QDir newDir(dir.path());
		newDir.cd(folder);

		// Check subfolder for ini file
		if (newDir.exists(folder+QString(".ini")))
		{
			QAction *newAct = new QAction(folder, this);
			signalMapper->setMapping(newAct, newDir.path());
			connect(newAct, &QAction::triggered, signalMapper, qOverload<>( &QSignalMapper::map ) );
			menu->addAction(newAct);
			//std::cout << "Pattern Found: " << folder.toStdString() << "\n";
		}
		// Recursively repeat
		else
		{
			QMenu *subMenu;
			subMenu = menu->addMenu(folder);
			findPatterns(subMenu, newDir);
		}
	}
}

QSize MainWindow::startupSize()
{
	QFile openFile("./Resources/Settings.txt");
	openFile.open(QIODevice::ReadOnly|QIODevice::Text);
	QTextStream stream(&openFile);
	QString line;
	int width=-1, height=-1;

	line = stream.readLine(100);
	while (stream.status()==QTextStream::Ok && !line.isNull())
	{
		if (line.contains("="))
		{
			QStringList list = line.split("=");
			QString id, value;
			id = QString(list.at(0).toLocal8Bit().constData());
			value = QString(list.at(1).toLocal8Bit().constData());
			if (id=="WindowWidth")
			{
				width = value.toInt();
			}
			else if (id=="WindowHeight")
			{
				height = value.toInt();
			}
		}
		line = stream.readLine(100);
	}
	if (width>0 && height>0)
		return QSize(width, height);

	return QSize(600,400);
}

void MainWindow::updateStatus()
{
	QString text = "";

	if (renderWidget->hasImageSelected())
		text += QString("Source Image Size: ")+QString::number(renderWidget->getImageSize().width())+QString("x")+QString::number(renderWidget->getImageSize().height())+QString("\t ");
	else
		text += QString("No Source Image\t ");

	if (renderWidget->hasPatternSelected())
		text += QString("Pattern: ")+renderWidget->getPatternName()+QString("\t ");
	else
		text += QString("No Pattern Selected\t ");

	if (renderWidget->hasImageSelected())
		text += QString("Total Tiles: ")+QString::number(renderWidget->getImageSize().width()*renderWidget->getImageSize().height());

	status->setText(text);
}
