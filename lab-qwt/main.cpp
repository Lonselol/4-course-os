#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_point_data.h>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        QWidget *centralWidget = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);

        plot = new QwtPlot(this);
        curve = new QwtPlotCurve("Temperature");
        curve->attach(plot);

        currentTempLabel = new QTextEdit(this);
        currentTempLabel->setReadOnly(true);
        layout->addWidget(currentTempLabel);

        startTimeEdit = new QLineEdit(this);
        startTimeEdit->setPlaceholderText("Start Time (YYYY-MM-DD HH:MM:SS)");
        layout->addWidget(startTimeEdit);

        endTimeEdit = new QLineEdit(this);
        endTimeEdit->setPlaceholderText("End Time (YYYY-MM-DD HH:MM:SS)");
        layout->addWidget(endTimeEdit);

        updateButton = new QPushButton("Update", this);
        layout->addWidget(updateButton);

        layout->addWidget(plot);

        setCentralWidget(centralWidget);

        connect(updateButton, &QPushButton::clicked, this, &MainWindow::updateData);

        networkManager = new QNetworkAccessManager(this);
    }

private slots:
    void updateData() {
        QString startTime = startTimeEdit->text();
        QString endTime = endTimeEdit->text();

        if (startTime.isEmpty() || endTime.isEmpty()) {
            fetchCurrentTemperature();
        } else {
            fetchTemperatureStats(startTime, endTime);
        }
    }

    void fetchCurrentTemperature() {
        QNetworkRequest request(QUrl("http://localhost:8080/current"));
        QNetworkReply *reply = networkManager->get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray response = reply->readAll();
                QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
                QJsonObject jsonObj = jsonDoc.object();
                int temperature = jsonObj["temperature"].toInt();
                currentTempLabel->setText(QString("Current Temperature: %1°C").arg(temperature));
            }
            reply->deleteLater();
        });
    }

    void fetchTemperatureStats(const QString &startTime, const QString &endTime) {
        QNetworkRequest request(QUrl(QString("http://localhost:8080/stats?start=%1&end=%2").arg(startTime, endTime)));
        QNetworkReply *reply = networkManager->get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray response = reply->readAll();
                QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
                QJsonObject jsonObj = jsonDoc.object();

                QJsonArray tempsArray = jsonObj["temperatures"].toArray();
                QVector<double> temps;
                for (const QJsonValue &value : tempsArray) {
                    temps.append(value.toInt());
                }

                QwtPointArrayData *data = new QwtPointArrayData(QVector<double>::fromList(QList<double>::fromVector(temps)), QVector<double>::fromList(QList<double>::fromVector(temps)));
                curve->setData(data);
                plot->replot();
            }
            reply->deleteLater();
        });
    }

private:
    QwtPlot *plot;
    QwtPlotCurve *curve;
    QTextEdit *currentTempLabel;
    QLineEdit *startTimeEdit;
    QLineEdit *endTimeEdit;
    QPushButton *updateButton;
    QNetworkAccessManager *networkManager;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

#include "main.moc"
