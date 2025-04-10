#include "ColorSlider.h"
#include <QLayout>
#include <QLinearGradient>
#include <QRect>
#include <QString>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

ColorSlider::ColorSlider( QWidget* parent, const QColor& color ) : QWidget( parent ) {
	QVBoxLayout* layout = new QVBoxLayout( this ) ;

	lineEdit = new QLineEdit( this ) ;
	slider = new QSlider( Qt::Vertical, this ) ;
	slider->setRange( 0, 255 ) ;
	slider->setMinimumHeight( 30 ) ;
	lineEdit->setPlaceholderText( "0" );
	QRegularExpression regex( "^([0-9]{1,3}|0x[0-9A-Fa-f]{1,2})$" ) ;

	lineEdit->setValidator( new QRegularExpressionValidator( regex, this ) ) ;
	lineEdit->setMaximumWidth( 30 ) ;

	layout->addWidget( lineEdit ) ;
	layout->addWidget( slider ) ;

	QString style = QString( 
		"QSlider::groove:vertical { "
		"  border: 1px solid #999999;"
		"  width: 10px; "
		"  background: qlineargradient(x1:0, y1:0, x2:0, y2:1 stop:0 %1, stop:1 black); "
        "}"
		"QSlider::handle:vertical { "
		"  border: 1px solid #5c5c5c;"
		"  width: 20px; "
		"  background: white; "
		"  height: 10px; "
		"  border-radius: 5px; "
		""
		"}"
	).arg( color.name() ) ;
	setStyleSheet( style );
	connect( slider, &QSlider::valueChanged, this, &ColorSlider::onSliderChanged ) ;
	connect( lineEdit, &QLineEdit::editingFinished, this, &ColorSlider::onTextChanged ) ;
}

int ColorSlider::value() const
{
	return slider->value();
}

void ColorSlider::setValue( int val ) {
	if ( val < 0 || val > 255 ) 
		return ;
	slider->setValue( val ) ;
	lineEdit->setText( QString::number( val ) ) ;
}

void ColorSlider::onTextChanged() {
	QString text = lineEdit->text().trimmed() ;
	bool ok ;
	int value = text.toInt( &ok, 10 ) ;
	if( !ok || value < 0 || value > 255 ) return ;
	slider->setValue( value ) ;
	emit valueChanged( value ) ;
}

void ColorSlider::onSliderChanged( int value ) {
	lineEdit->setText( QString::number( value ) ) ;
	emit valueChanged( value ) ;
}