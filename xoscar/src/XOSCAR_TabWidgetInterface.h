#ifndef XOSCAR_TABWIDGETINTERFACE_H
#define XOSCAR_TABWIDGETINTERFACE_H
#include <QWidget>

class XOSCAR_TabWidgetInterface
{

public:
	XOSCAR_TabWidgetInterface();
	~XOSCAR_TabWidgetInterface();

	virtual bool save() = 0;
    virtual bool undo() = 0;

    virtual bool isModified() const { return modified; }

signals:
    virtual void widgetContentsModified(QWidget* widget)=0;

protected:
	virtual void setModified(const bool mod) { modified = mod; }
    bool modified;
};

#endif // XOSCAR_TABWIDGETINTERFACE_H
