#include <QtWidgets>
#include <utility>
#include "Renderer.h"

Renderer::Renderer(QWidget *parent)
    : QWidget(parent), hasPattern(false), hasImage(false), antialiased(true), paintedBackground(false),
    hasImageChanged(false), erasedRing(false), gridX(1), gridY(1), outline(Qt::black), leftClickBrush(Qt::black), 
	rightClickBrush(Qt::white), background(Qt::white), paintedScene(new QPixmap(1,1))
{
	this->setAutoFillBackground(false);
	/*QPalette p = this->palette();
	p.setColor(this->backgroundRole(), background);
	this->setPalette(p);
	this->setAutoFillBackground(true);*/
}

Renderer::~Renderer()
{
	if (hasImage && hasPattern)
	{
		delete paintedScene;
		hasImage = false;
	}
    else if(paintedScene)
    {
        delete paintedScene ;
    }
}

void Renderer::newImage(int x, int y)
{
	image = QImage(x,y,QImage::Format_RGB32);
	image.fill( Qt::white ) ;
	//image.fill(leftClickBrush.rgb());

	gridX = x;
	gridY = y;
	hasImage = true;
	hasImageChanged = true;
	paintedBackground = false;
	QRegion newRegion;
	paintedRegion.swap(newRegion);
	
	updatePatternSize();
	update();
}

bool Renderer::loadImage(QString imagePath, bool redraw)
{
	image.load(imagePath);
	if (!image.isNull())
	{
		gridX = image.width();
		gridY = image.height();;

		hasImage = true;
		hasImageChanged = false;
		updatePatternSize();

		if (redraw)
		{
			paintedBackground = false;
			QRegion newRegion;
			paintedRegion.swap(newRegion);
			update();
		}
		return true;
	}
	return false;
}

bool Renderer::smartResize(int x, int y)
{
	QImage imageSource;
	imageSource = image;
	if (!imageSource.isNull() && hasPattern)
	{
		int pixelXCount = x/pattern.getReadX();
		int pixelYCount = y/pattern.getReadY();
		if (pixelXCount%pattern.getReadX()==0)
			pixelXCount--;
		if (pixelYCount%pattern.getReadY()==0)
			pixelYCount--;

		pixelXCount = pattern.getX()*pixelXCount+pattern.getLargestTileOffsetX();
		pixelYCount = pattern.getY()*pixelYCount+pattern.getLargestTileOffsetY();

		image = QImage(x, y, QImage::Format_RGB32);

		for (int i=0; i<x; i++)
		{
			for (int j=0; j<y; j++)
			{
				int tileX = i%pattern.getReadX();
				int tileY = j%pattern.getReadY();
				float displaceX = pattern.getX()*(i/pattern.getReadX())+pattern.getTileW(tileX, tileY)/2.0+pattern.getTileX(tileX,tileY);
				float displaceY = pattern.getY()*(j/pattern.getReadY())+pattern.getTileH(tileX, tileY)/2.0+pattern.getTileY(tileX,tileY);
				int ii = int(imageSource.width()/float(pixelXCount)*displaceX);
				int jj = int(imageSource.height()/float(pixelYCount)*displaceY);

				if (ii > imageSource.width()-1)
					ii = imageSource.width()-1;
				if (jj > imageSource.height()-1)
					jj = imageSource.height()-1;
				image.setPixel(i, j, imageSource.pixel(ii, jj));
			}
		}

		// Set the remainder of the values
		gridX = image.width();
		gridY = image.height();

		hasImage = true;
		hasImageChanged = true;
		paintedBackground = false;
		updatePatternSize();
		QRegion newRegion;
		paintedRegion.swap(newRegion);
		update();
		return true;
	}
	return false;
}

QSize Renderer::smartResizeTip()
{
	if (hasPattern && hasImage)
	{
		return QSize(image.width()*pattern.getReadX()*20/pattern.getX(),image.height()*pattern.getReadY()*20/pattern.getY());
	}
	if (hasImage)
		return QSize(image.size());
	return QSize(1,1);
}

void Renderer::resizeImage(int x, int y)
{
	if (!image.isNull())
	{
		image = image.scaled(x,y);
		
		gridX = image.width();
		gridY = image.height();

		paintedBackground = false;
		hasImageChanged = true;
		updatePatternSize();
		QRegion newRegion;
		paintedRegion.swap(newRegion);
		update();
	}
}

void Renderer::changePalette(QString colorFile)
{
	if (!image.isNull())
	{
		QVector<QRgb> newColorTable;
		QImage paletteImage;
		paletteImage.load(colorFile);
		if (!paletteImage.isNull())
		{
			for (int i=0; i<paletteImage.width(); i++)
			{
				newColorTable.push_back(paletteImage.pixel(i,0));
			}
			image = image.convertToFormat(QImage::Format_Indexed8, newColorTable, Qt::ColorOnly);
			image = image.convertToFormat(QImage::Format_RGB32, newColorTable, Qt::ColorOnly);
			QRegion newRegion;
			paintedRegion.swap(newRegion);
			hasImageChanged = true;
			update();
		}
	}
}

void Renderer::setBackgroundColor(QColor color)
{
	background = color;
	paintedBackground = false;
	QRegion newRegion;
	paintedRegion.swap(newRegion);
	update();
}

void Renderer::setOutlineColor(QColor color)
{
	outline = color;
	paintedBackground = false;
	QRegion newRegion;
	paintedRegion.swap(newRegion);
	update();
}

void Renderer::setBrushColor(QColor color)
{
	leftClickBrush = color ;
}

void Renderer::repaintImage()
{
	QRegion newRegion;
	paintedRegion.swap(newRegion);
	repaint(0,0,sizeX,sizeY);
	// doesnt quite repaint the entire image at once
}

void Renderer::rotateTranspose()
{
	rotate(QTransform(0,-1,-1,0,1,1));
}

void Renderer::rotateClockwise()
{
	rotate(QTransform(0,1,-1,0,1,1));
}

void Renderer::rotateCounterClockwise()
{
	rotate(QTransform(0,-1,1,0,1,1));
}

void Renderer::rotate180()
{
	rotate(QTransform(-1,0,0,-1,1,1));
}

void Renderer::rotateFlipX()
{
	rotate(QTransform(-1,0,0,1,1,1));
}

void Renderer::rotateFlipY()
{
	rotate(QTransform(1,0,0,-1,1,1));
}

void Renderer::zoomIn()
{
	zoom*=1.1;
	updatePatternSize();
	QTransform trans(1.1,0.0,0.0,1.1,1.0,1.0);
	paintedRegion = trans.map(paintedRegion);
	paintedRegion.translate(QPoint(-1,-1));
	update();
}

void Renderer::zoomOut()
{
	zoom*=0.9;
	updatePatternSize();
	QTransform trans(0.9,0.0,0.0,0.9,1.0,1.0);
	paintedRegion = trans.map(paintedRegion);
	paintedRegion.translate(QPoint(-1,-1));
	update();
}

void Renderer::zoomNormal()
{
	zoom=1.0;
	updatePatternSize();
	update();
}

bool Renderer::setPattern(QDir dir)
{
	if (pattern.loadPattern(dir))
	{
		zoom = 1;
		hasPattern = true;
		updatePatternSize();
		paintedBackground = false;
		QRegion newRegion;
		paintedRegion.swap(newRegion);
		update();
		return true;
	}
	hasPattern = false;
	updatePatternSize();
	return false;
}

QSize Renderer::getMinSize()
{
	updatePatternSize();
	return QSize(sizeX,sizeY);
}

QSize Renderer::getImageSize()
{
	if (!image.isNull())
		return image.size();
	return QSize(1,1);
}

bool Renderer::saveImage(QString saveFileName)
{
	if (hasImage)
	{
		bool wasSaved = image.save(saveFileName);
		if (wasSaved)
			hasImageChanged = false;
		return wasSaved;
	}
	return false;
}

bool Renderer::imageChanged()
{
	if (hasImage && hasPattern)
		return hasImageChanged;
	return false;
}

bool Renderer::hasPatternSelected()
{
	return hasPattern;
}

bool Renderer::hasImageSelected()
{
	return hasImage;
}

QString Renderer::getPatternName()
{
	if (hasPattern)
		return pattern.getPatternName();
	return QString("None");
}

void Renderer::countColors(QVector<QRgb> *colors, QVector<int> *counts)
{
	if (hasImage)
	{
		colors->clear();
		counts->clear();
		QImage newImage = image.convertToFormat(QImage::Format_Indexed8, Qt::ColorOnly);
		
		for (int i=0; i<image.width(); i++)
		{
			for (int j=0; j<image.height(); j++)
			{
				int location = colors->indexOf(image.pixel(i,j));
				if (location >= 0)
				{
					counts->replace(location,counts->value(location)+1);
				}
				else
				{
					colors->push_back(image.pixel(i,j));
					counts->push_back(1);
				}
			}
		}
	}
}

void Renderer::paintEvent(QPaintEvent *e)
{
	QPainter painter;
	painter.begin(paintedScene);

	if (hasPattern && hasImage)
	{
		// Repaint everything if background needs repainting
		if (!paintedBackground)
		{
			QRegion newRegion;
			paintedRegion.swap(newRegion);
			painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
			painter.fillRect(QRect(0,0,sizeX/zoom+1,sizeY/zoom+1),background);

			// Old outline format support
			if (!pattern.getTransparencySupport())
			{
				painter.setPen(outline);
				painter.drawTiledPixmap(0,0,paintedScene->rect().right()+1,paintedScene->rect().bottom()+1,QBitmap(pattern.getBackground()));
			}
			paintedBackground = true;
		}
		QRegion remainingRegion;
		remainingRegion = e->region().subtracted(paintedRegion);
		int x=0;
		int y=0;
		if (!remainingRegion.isEmpty())
		{
			paintedRegion += e->region();
			// Without transparency, -1/+2 works for repainting grids
			int gridCountX = int(gridX/pattern.getReadX());
			int gridXStart = int(remainingRegion.boundingRect().left()/zoom/pattern.getX())-1;
			int gridXEnd = int(remainingRegion.boundingRect().right()/zoom/pattern.getX())+2;
			int gridCountY = int(gridY/pattern.getReadY());
			int gridYStart = int(remainingRegion.boundingRect().top()/zoom/pattern.getY())-1;
			int gridYEnd = int(remainingRegion.boundingRect().bottom()/zoom/pattern.getY())+2;

			if (gridXStart<0)
				gridXStart=0;
			if (gridYStart<0)
				gridYStart=0;
			if (gridXEnd>gridCountX)
				gridXEnd=gridCountX;
			if (gridYEnd>gridCountY)
				gridYEnd=gridCountY;

			// Paint supporting transparency
			if (pattern.getTransparencySupport())
			{
				// Clear background for erased rings
				if (erasedRing)
				{
					QRect backgroundRegion;
					// Region not quite right, but pretty close.
					backgroundRegion=QRect(remainingRegion.boundingRect().x()/zoom,remainingRegion.boundingRect().y()/zoom,remainingRegion.boundingRect().width()*3/zoom,remainingRegion.boundingRect().height()*3/zoom);
					painter.fillRect(backgroundRegion,background);
					erasedRing=false;
				}
				// Paint lowest levels first
				for (int k=pattern.getLayerCount(); k>=0; k--)
				{
					int pyramid=pattern.getLayerCount()-k;
					int endX=(gridXEnd+pyramid<gridCountX)?gridXEnd+pyramid:gridCountX;
					for (int i=gridXStart-pyramid; i<=endX; i++)
					{
						if (i<0)
							i=0;
						x=int(pattern.getX())*i;
						int endY=(gridYEnd+pyramid<gridCountY)?gridYEnd+pyramid:gridCountY;
						for (int j=gridYStart-pyramid; j<=endY; j++)
						{
							if (j<0)
								j=0;
							y=int(pattern.getY())*j;

							// Paint tiles
							painter.save();
							painter.translate(x,y);
							for (int tileX=0; tileX < pattern.getReadX(); tileX++)
							{
								for (int tileY=0; tileY < pattern.getReadY(); tileY++)
								{
									int pixelX = i*(pattern.getReadX())+pattern.getTileReadX(tileX, tileY);
									int pixelY = j*(pattern.getReadY())+pattern.getTileReadY(tileX, tileY);
									if (pixelX < gridX && pixelY < gridY)
									{
										if (qAlpha(image.pixel(pixelX, pixelY))>127)
										{
											QPixmap tilePixmap = pattern.getTile(tileX,tileY);
											QBitmap mask1 = tilePixmap.createMaskFromColor(QColor(k,k,k));
											QBitmap mask2 = tilePixmap.createMaskFromColor(QColor(255-k,255-k,255-k));

											// Paint masks
											tilePixmap.fill(image.pixel(pixelX,pixelY));
											tilePixmap.setMask(mask1.createMaskFromColor(Qt::black));
											painter.drawPixmap(pattern.getTileX(tileX, tileY),pattern.getTileY(tileX, tileY), tilePixmap);

											tilePixmap.fill(outline);
											tilePixmap.setMask(mask2.createMaskFromColor(Qt::black));
											painter.drawPixmap(pattern.getTileX(tileX, tileY),pattern.getTileY(tileX, tileY), tilePixmap);
										}
									}
								}
							}
							painter.restore();
						}
					}
				}
			}

			// Old File Support
			else
			{
				for (int i=gridXStart; i<=gridXEnd; i++)
				{
					x=int(pattern.getX())*i;
					for (int j=gridYStart; j<=gridYEnd; j++)
					{
						y=int(pattern.getY())*j;

						// Paint tiles
						painter.save();
						painter.translate(x,y);
						for (int tileX=0; tileX < pattern.getReadX(); tileX++)
						{
							for (int tileY=0; tileY < pattern.getReadY(); tileY++)
							{
								int pixelX = i*(pattern.getReadX())+pattern.getTileReadX(tileX, tileY);
								int pixelY = j*(pattern.getReadY())+pattern.getTileReadY(tileX, tileY);
								if (pixelX < gridX && pixelY < gridY)
								{
									//painter.setBackgroundMode(Qt::TransparentMode);
									QPixmap tilePixmap = pattern.getTile(tileX,tileY);
									QBitmap mask = tilePixmap.createMaskFromColor(Qt::white);
									tilePixmap.fill(qAlpha(image.pixel(pixelX, pixelY))>127?image.pixel(pixelX,pixelY):background);
									tilePixmap.setMask(mask);
									painter.drawPixmap(pattern.getTileX(tileX, tileY),pattern.getTileY(tileX, tileY), tilePixmap);
								}
							}
						}
						painter.restore();
					}
				}
			}
		}
		// Paint on the widget
		painter.end();
		painter.begin(this);
		painter.scale(zoom,zoom);
		painter.drawPixmap(paintedScene->rect(),*paintedScene,paintedScene->rect());
		painter.end();
	}
}

void Renderer::mousePressEvent(QMouseEvent *e)
{
	QColor targetColor ;
	if( e->button() == Qt::LeftButton ) {
		targetColor = leftClickBrush ;
	}
	else if( e->button() == Qt::RightButton ) {
		targetColor = rightClickBrush ;
	}
	else if( e->button() == Qt::MiddleButton ) {
		targetColor = Qt::white ;
	}
	if (hasImage && hasPattern)
	{
		// Starting positions to test
		int eX = int(e->x()/zoom);
		int eY = int(e->y()/zoom);
		int startX = int(e->x()/zoom/pattern.getX());
		int startY = int(e->y()/zoom/pattern.getY());

		// Search 3x3 patterned area
		for (int i=-1; i<2; i++)
		{
			for (int j=-1; j<2; j++)
			{
				int curX = i+startX;
				int curY = j+startY;

				if (curX >= 0 && curX < gridX && curY >= 0 && curY < gridY)
				{
					// Search all subtiles
					for (int tileX=0; tileX<pattern.getReadX(); tileX++)
					{
						for (int tileY=0; tileY<pattern.getReadY(); tileY++)
						{
							//QBitmap mask = pattern.getTile(tileX,tileY);
							QBitmap mask = (QBitmap) pattern.getClickMask(tileX,tileY);
							int xTest = eX-(curX*pattern.getX()+pattern.getTileX(tileX,tileY));
							int yTest = eY-(curY*pattern.getY()+pattern.getTileY(tileX,tileY));
							if (mask.rect().contains(xTest,yTest))
							{
								// Check if clicked pixel is within tile mask
								// Should loop to find smallest qGray value on tiles searched.
								if (qGray(mask.toImage().pixel(xTest,yTest))==0 /*&& qGray(tile.mask().toImage().pixel(xTest,yTest))==0*/)
								{
									int pixelX = curX*pattern.getReadX()+pattern.getTileReadX(tileX,tileY);
									int pixelY = curY*pattern.getReadY()+pattern.getTileReadY(tileX,tileY);
									// Make sure clicked image pixel is inside the image
									if (pixelX < gridX && pixelY < gridY)
									{
										image.setPixel(pixelX,pixelY,targetColor.rgba());
										QRegion repaintRegion(curX*pattern.getX()*zoom,curY*pattern.getY()*zoom,pattern.getLargestTileOffsetX()*zoom,pattern.getLargestTileOffsetY()*zoom);
										paintedRegion-=repaintRegion;
										erasedRing=true;
										repaint(curX*pattern.getX()*zoom,curY*pattern.getY()*zoom,pattern.getLargestTileOffsetX()*zoom,pattern.getLargestTileOffsetY()*zoom);
										hasImageChanged = true;
									}
								}
							}
						}
					}
				}
			}	
		}
	}
}


void Renderer::swapBrush()
{
	std::swap( leftClickBrush, rightClickBrush ) ;
}
	
void Renderer::updateBrushColor( QColor color ) {
	setBrushColor( color ) ;
}

void Renderer::updatePatternSize()
{
	if (hasImage && hasPattern)
	{
		float gridXCount = gridX/pattern.getReadX();
		float gridYCount = gridY/pattern.getReadY();
		if (gridX%pattern.getReadX()==0)
			gridXCount--;
		if (gridY%pattern.getReadY()==0)
			gridYCount--;

		// Prior to zoom
		sizeX = int((pattern.getX()*gridXCount+pattern.getLargestTileOffsetX()));
		sizeY = int((pattern.getY()*gridYCount+pattern.getLargestTileOffsetY()));
		if (paintedScene->size()!=QSize(sizeX,sizeY))
		{
			delete paintedScene;
			paintedScene = new QPixmap(sizeX, sizeY);
		}
		// Apply zoom
		sizeX = sizeX*zoom;
		sizeY = sizeY*zoom;
	}
	else
	{
		sizeX = 1;
		sizeY = 1;
	}
}

void Renderer::rotate(QTransform matrix)
{
	if (hasImage)
	{
		QImage newImage = image.transformed(matrix);
		image.swap(newImage); 

		gridX = image.width();
		gridY = image.height();
		
		updatePatternSize();
		paintedBackground = false;
		QRegion newRegion;
		paintedRegion.swap(newRegion);
		paintedScene = new QPixmap(sizeX, sizeY);
		update();
		hasImageChanged = true;
	}
}


