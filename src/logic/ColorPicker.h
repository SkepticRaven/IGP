#pragma once
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "ColorSlider.h"
#include <QEvent>
#include <QObject>
#include <QColor>

class ColorButton : public QPushButton {
	Q_OBJECT
public:
	explicit ColorButton( QWidget* parent = nullptr, bool selected = false, const QColor& start = Qt::black ) ;
	void setButtonColor( const QColor& color ) ;
	void setLabelColor( const QColor& color ) ;
	void setInactive() ;
	QColor getColor() ;
signals:
	void selectionChanged( bool selected ) ;
public slots:
	void onClick() ;
private:
	QLabel* innerColor ;
	QColor currentColor ;
	QColor borderColor ;
	bool selected ;
};

class ColorPicker : public QWidget {
	Q_OBJECT
public:
	explicit ColorPicker( QWidget* parent = nullptr ) ;
	void overrideSelectedColor( QColor color ) ;

signals:
	void colorChanged( QColor color ) ;
	void swappedButton() ;

protected:
	bool eventFilter( QObject* obj, QEvent* event ) override ;

private slots:
	void updateColorFromSlider() ;
	void updateColorFromHex() ;
	void activateEyedropper() ;
	void togglePicker() ;
	void handleButtonSelection( bool selected );

private:
	void setColor( const QColor& color, bool emitter = true ) ;
	// Sliders
	ColorButton* leftButton ;
	ColorButton* rightButton ;
	ColorButton* active ;
	QLineEdit* hexcode ;
	ColorSlider* red ;
	ColorSlider* green ;
	ColorSlider* blue ;
	// Eyedropper
	QPushButton* eyedropper ; 
	bool pickingColor ;
	QWidget* colorPickerWidget ;
	// Collapse
	QPushButton* toggleButton ;
};
