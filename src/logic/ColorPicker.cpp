#include "ColorPicker.h"
#include <QBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QSlider>
#include <QPoint>
#include <QPalette>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QApplication>
#include <QMouseEvent>
#include <QScreen>
#include <QSignalBlocker>


ColorPicker::ColorPicker( QWidget* parent )
{
	QHBoxLayout* mainLayout = new QHBoxLayout( this ) ;
	pickingColor = false ;

	toggleButton = new QPushButton( "<<", this ) ;
	toggleButton->setFixedWidth( 20 ) ;
	toggleButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding ) ;
	mainLayout->addWidget( toggleButton ) ;

	

	colorPickerWidget = new QWidget( this ) ;
	QBoxLayout* pickerLayout = new QVBoxLayout( colorPickerWidget ) ;
	pickerLayout->setContentsMargins( 0, 0, 0, 0 ) ;
	colorPickerWidget->setVisible( false ) ;
	colorPickerWidget->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Expanding ) ;
	mainLayout->addWidget( colorPickerWidget ) ;

	QBoxLayout* labelLayout = new QHBoxLayout() ;

	leftButton = new ColorButton( this, true ) ;
	active = leftButton ; // Set leftButton as active in order to allow proper changes.
	rightButton = new ColorButton( this, false, Qt::white ) ;

	connect( leftButton, &ColorButton::selectionChanged, this, &ColorPicker::handleButtonSelection ) ;
	connect( rightButton, &ColorButton::selectionChanged, this, &ColorPicker::handleButtonSelection ) ;
	labelLayout->addWidget( leftButton ) ;
	labelLayout->addWidget( rightButton ) ;
	pickerLayout->addLayout( labelLayout ) ;


	// Generate the RGB
	QHBoxLayout* sliderLayout = new QHBoxLayout() ;

	red = new ColorSlider( this, Qt::red ) ;
	green = new ColorSlider( this, Qt::green ) ;
	blue = new ColorSlider( this, Qt::blue ) ;

	sliderLayout->addWidget( red ) ;
	sliderLayout->addWidget( green ) ;
	sliderLayout->addWidget( blue ) ;

	pickerLayout->addLayout( sliderLayout ) ;

	connect( red, &ColorSlider::valueChanged, this, &ColorPicker::updateColorFromSlider );
	connect( blue , &ColorSlider::valueChanged, this, &ColorPicker::updateColorFromSlider );
	connect( green, &ColorSlider::valueChanged, this, &ColorPicker::updateColorFromSlider );

	// Set up the defaults for the hex input
	hexcode = new QLineEdit( this ) ;
	hexcode->setAlignment( Qt::AlignCenter ) ;
	hexcode->setPlaceholderText( "#RRGGBB" ) ;
	hexcode->setMaxLength( 7 );
	QRegularExpression hexregex( "^#?([A-Fa-f0-8]{6})$" ) ;
	hexcode->setValidator( new QRegularExpressionValidator( hexregex, this ) ) ;

	pickerLayout->addWidget( hexcode ) ;

	connect( hexcode, &QLineEdit::editingFinished, this, &ColorPicker::updateColorFromHex ) ;

	// Create the eyedropper
	eyedropper = new QPushButton( "Pick Color", this ) ;
	eyedropper->setIcon( QIcon("./Resources/eyedropper.svg"));
	eyedropper->setIconSize( QSize( 16, 16 ) );
	pickerLayout->addWidget( eyedropper ) ;

	connect( toggleButton, &QPushButton::clicked, this, &ColorPicker::togglePicker ) ;
	connect( eyedropper, &QPushButton::clicked, this, &ColorPicker::activateEyedropper ) ;
	qApp->installEventFilter( this ) ;

	updateColorFromSlider() ;
}


void ColorPicker::overrideSelectedColor( QColor color ) {
	setColor( color ) ;
}


bool ColorPicker::eventFilter( QObject* obj, QEvent* event )
{
	if( pickingColor && event->type() == QEvent::MouseButtonPress ) {
		QMouseEvent* mouseEvent = static_cast< QMouseEvent * >( event ) ;
		pickingColor = false ;
		QApplication::setOverrideCursor( Qt::ArrowCursor ) ;
		QPoint position = mouseEvent->globalPos() ;
		QScreen* screen = QApplication::screenAt( position ) ;
		if( !screen ) return false ;

		QPixmap image = screen->grabWindow( 0, position.x() - screen->geometry().x(), position.y() - screen->geometry().y(), 1, 1);

		QColor pickedColor = image.toImage().pixelColor( 0, 0 ) ;
		if( pickedColor.isValid() ) {
			red->setValue( pickedColor.red() ) ;
			green->setValue( pickedColor.green() ) ;
			blue->setValue( pickedColor.blue() ) ;

			updateColorFromSlider() ;
		}
		return true ;
	}

	return QWidget::eventFilter( obj, event ) ;
}


void ColorPicker::updateColorFromSlider() {
	QColor color( 
		red->value(), 
		green->value(), 
		blue->value() 
	) ;
	setColor( color ) ;
}


void ColorPicker::updateColorFromHex() {
	QString hex = hexcode->text().trimmed() ;
	if( !hex.startsWith( "#" ) )
		hex.prepend( "#" ) ;
	QColor color( hex ) ;
	if( color.isValid()) {
		setColor( color ) ;
	}
}


void ColorPicker::activateEyedropper() {
	pickingColor = true ;
	QApplication::setOverrideCursor( Qt::CrossCursor ) ;
}


void ColorPicker::togglePicker()
{
	bool visible = colorPickerWidget->isVisible() ;
	colorPickerWidget->setVisible( !visible ) ;
	toggleButton->setText( visible ? "<<" : ">>" ) ;
}


void ColorPicker::handleButtonSelection( bool selected )
{
	ColorButton* previous = active ;
	if( sender() == leftButton ) 
	{
		active = leftButton ;
		leftButton->setButtonColor( Qt::blue ) ;
		rightButton->setInactive() ;
		setColor( leftButton->getColor(), false ) ;
	}
	else if( sender() == rightButton )
	{
		active = rightButton ;
		rightButton->setButtonColor( Qt::blue ) ;
		leftButton->setInactive() ;
		setColor( rightButton->getColor(), false ) ;
	}
	if( previous != active ) {
		emit swappedButton() ;
	}
}


void ColorPicker::setColor( const QColor& color, bool emitter )
{
	if( emitter ) {
		red->setValue( color.red() ) ;
		green->setValue( color.green() ) ;
		blue->setValue( color.blue() ) ;
	}
	else {
		const QSignalBlocker blockr( red ) ;
		const QSignalBlocker blockg( green ) ;
		const QSignalBlocker blockb( blue ) ;
		red->setValue( color.red() ) ;
		green->setValue( color.green() ) ;
		blue->setValue( color.blue() ) ;
	}
	active->setLabelColor( color ) ;
	hexcode->setText( color.name( QColor::HexRgb ) ) ;
	if( emitter )
	{
		emit colorChanged( color ) ;
	}
}


ColorButton::ColorButton( QWidget* parent, bool selected, const QColor& defaultColor ) : QPushButton(parent), currentColor(defaultColor), selected( selected )
{
	innerColor = new QLabel( this ) ;
	innerColor->setBackgroundRole( QPalette::Window ) ;
	innerColor->setAlignment( Qt::AlignCenter ) ;
	innerColor->setAutoFillBackground( true ) ;
	setMinimumSize( 45, 45 ) ;
	setMaximumWidth( 300 ) ;
	

	QBoxLayout* layout = new QVBoxLayout( this ) ;
	layout->setContentsMargins( 5, 5, 5, 5 ) ;
	layout->addWidget( innerColor ) ;

	setLayout( layout ) ;
	setAutoFillBackground( true ) ;
	if( selected )
		setButtonColor( Qt::blue ) ;
	else
		setButtonColor( Qt::gray ) ;
	setLabelColor( defaultColor ) ;
	connect( this, &ColorButton::clicked, this, &ColorButton::onClick ) ;
}


void ColorButton::setButtonColor( const QColor& color )
{
	borderColor = color ;
	QPalette pal = palette() ;
	pal.setColor( QPalette::Button, color ) ;
	setPalette( pal ) ;
	update() ;
}


void ColorButton::setLabelColor( const QColor &color )
{
	currentColor = color ;
	QPalette palette = innerColor->palette() ;
	palette.setColor( QPalette::Window, color ) ;
	innerColor->setPalette( palette ) ;
	update() ;
}


void ColorButton::setInactive()
{
	selected = false ;
	setButtonColor( Qt::gray ) ;
}


QColor ColorButton::getColor()
{
	return currentColor ;
}


void ColorButton::onClick()
{
	if( !selected ) {
		selected = !selected ;
		emit selectionChanged( selected ) ;
	}
}