#ifndef  CUSTOMFONT_H
# define CUSTOMFONT_H

# include <QObject>
# include <QString>

class CustomFont : public QObject
{
  Q_OBJECT
  Q_PROPERTY(bool    enabled   MEMBER enabled   NOTIFY enabledChanged)
  Q_PROPERTY(QString source    MEMBER source    NOTIFY sourceChanged)
  Q_PROPERTY(QString uri       READ getUri      NOTIFY sourceChanged)
  Q_PROPERTY(double  pixelSize MEMBER pixelSize NOTIFY sizeChanged)
public:
  CustomFont(QObject* parent = nullptr);

  QString getUri() const;

signals:
  void enabledChanged();
  void sourceChanged();
  void sizeChanged();
private:
  bool enabled = false;
  QString source;
  double pixelSize = 12;
};

#endif
