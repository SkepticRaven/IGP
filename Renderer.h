#ifndef RENDERER_H
#define RENDERER_H

#include <QPixmap>
#include <QWidget>
#include <QScrollArea>
#include <QLabel>
#include <QTransform>
#include <QDir>
#include "Pattern.h"

class Renderer : public QWidget
{
	Q_OBJECT

public:
	Renderer(QWidget *parent = 0);
	~Renderer();

	void loadImage(QString imagePath);
	void rotateImage();
	void zoomIn();
	void zoomOut();
	void zoomNormal();
	void setPattern(QDir dir);
	QSize getMinSize();
	bool saveImage(QString saveFileName);

protected:
    void paintEvent(QPaintEvent *event);

private:
	// Helper Functions
	void updatePatternSize();

	// Painting Variables
	bool antialiased;
	bool hasPattern;
	bool hasImage;
	Pattern pattern;
	double zoom;
	int gridX;
	int gridY;
	int sizeX;
	int sizeY;
	QImage image;
};

#endif
