#include "gaussjordan.h"

GaussJordan::GaussJordan(QObject *parent, int precision) : QObject(parent) {
    this->precision = precision;
    this->st = 0;
}

void GaussJordan::pass_data(QString data) {
    this->st = 0;

    if(load_data(data)){
        compute_standard();
        try {
            compute_interval();
        } catch(...) {
            iComputed = false;
            this->st = -1;
        }
    }
}

bool GaussJordan::load_data(QString data) {
    sMatrix.erase(sMatrix.begin(),sMatrix.end());
    iMatrix.erase(iMatrix.begin(),iMatrix.end());
    sColumns.erase(sColumns.begin(),sColumns.end());
    iColumns.erase(iColumns.begin(),iColumns.end());

    QRegExp rx("(\\ |\\t|\\n|\\+)");
    QRegExp iRx("(\\[((\\-)?[0-9]+(\\.)?[0-9]*)\\,((\\-)?[0-9]+(\\.)?[0-9]*)\\])");
    data.replace(rx,"");

    rx.setPattern("(((((\\-)?[0-9]+(\\.)?[0-9]*\\_)|(\\[((\\-)?[0-9]+(\\.)?[0-9]*)\\,((\\-)?[0-9]+(\\.)?[0-9]*)\\]\\_))*(((\\-)?[0-9]+(\\.)?[0-9]*)|(\\[((\\-)?[0-9]+(\\.)?[0-9]*)\\,((\\-)?[0-9]+(\\.)?[0-9]*)\\]))\\=(((\\-)?[0-9]+(\\.)?[0-9]*)||(\\[((\\-)?[0-9]+(\\.)?[0-9]*)\\,((\\-)?[0-9]+(\\.)?[0-9]*)\\]))\\;)+)");
    if(!rx.exactMatch(data)) {
        this->st = 1;
        return false;
    }
    rx.setPattern("(\\;)");
    QStringList query = data.split(rx);

    rx.setPattern("(\\_|\\=)");

    unsigned int n = query.size() - 1;

    for (unsigned int i = 0; i < n; i++) {
        vector<double> sRow;
        vector<Interval<double>> iRow;
        QStringList values = query.at(i).split(rx);
        if(values.size() != n + 1) {
            this->st = 1;
            return false;
        }
        for(int j = 0; j < values.size();j++) {
            if(!iRx.exactMatch(values.at(j))) {
                sRow.push_back(values.at(j).toDouble());
                iRow.push_back(Interval<double>::IntRead(values.at(j).toStdString()));
            }else{
                QString intStr = values.at(j);
                intStr.replace("[", "");
                intStr.replace("]", "");
                QStringList iList = intStr.split(',');
                if(iList.size() != 2) {
                    this->st = 1;
                    return false;
                }
                Interval<double> tmp;
                if(iList.at(0).toDouble() > iList.at(1).toDouble()) {
                    this->st = 1;
                    return false;
                }
                sRow.push_back((iList.at(0).toDouble() + iList.at(1).toDouble()) / 2);
                tmp.a = Interval<double>::LeftRead(iList.at(0).toStdString());
                tmp.b = Interval<double>::RightRead(iList.at(1).toStdString());
                iRow.push_back(tmp);
            }
        }
        if(sRow.size() != n + 1 || iRow.size() != n + 1) {
            this->st = 2;
            sMatrix.erase(sMatrix.begin(),sMatrix.end());
            iMatrix.erase(iMatrix.begin(),iMatrix.end());
            return false;
        }

        sMatrix.push_back(sRow);
        iMatrix.push_back(iRow);
        sColumns.push_back(i);
        iColumns.push_back(i);
    }

    return true;
}

QString GaussJordan::results_standard() {
    if(this->st > 0) return "Error: " + QString::number(this->st);
    if(!sComputed) return "Results has not been computed yet!";

    QString result;

    for(unsigned int i = 0; i < sMatrix.size(); i++){
        unsigned int el = i;
        for(unsigned int j = 0; j < sColumns.size(); j++){
            if(sColumns[j] == i) {
                el = j;
                break;
            }
        }
       result += "X" + QString::number(i) + " = " + QString::number(sMatrix[el][sMatrix[el].size() - 1],'f', this->precision) + "\n";
    }

    return result;
}

QString GaussJordan::results_interval() {
    if(this->st != 0) return "Error: " + QString::number(this->st);
    if(!iComputed) return "Results has not been computed yet!";

    QString result;

    for(unsigned int i = 0; i < iMatrix.size(); i++){
        unsigned int el = i;
        for(unsigned int j = 0; j < iColumns.size(); j++){
            if(iColumns[j] == i) {
                el = j;
                break;
            }
        }

        string a,b;
        iMatrix[el][iMatrix[el].size() - 1].IEndsToStrings(a, b);
        result += "X" + QString::number(i) + " = [" + QString::fromStdString(a) + ", " + QString::fromStdString(b) + "]\n";
    }

    return result;
}

QString GaussJordan::print_sMatrix()
{
    QString text = "Standard Matrix:\n";

    for(unsigned int i = 0; i < sMatrix.size(); i++){
       for(unsigned int j = 0; j < sMatrix[i].size(); j++) text += ((j == sMatrix[i].size() - 1) ? "|  " : "") + QString::number(sMatrix[i][j],'f', this->precision) + "\t";
       text += "\n";
    }

    return text;
}

QString GaussJordan::print_iMatrix()
{
    QString text = " Interval Matrix:\n";

    for(unsigned int i = 0; i < iMatrix.size(); i++){
       for(unsigned int j = 0; j < iMatrix[i].size(); j++) {
           string a,b;
           iMatrix[i][j].IEndsToStrings(a, b);
           text += ((j == iMatrix[i].size() - 1) ? "|  [" : " [") + QString::fromStdString(a) + ", " + QString::fromStdString(b) + "]\t";
       }
       text += "\n";
    }

    return text;
}

void GaussJordan::compute_standard() {
    for(unsigned int i = 0; i < sMatrix.size(); i++) {
        QPoint firstElement = maxElement(true, i);
        if(sMatrix[firstElement.x()][firstElement.y()] == 0){
            this->st = 3;
            return;
        }
        //swap rows
        if(firstElement.x() != i) sMatrix[i].swap(sMatrix[firstElement.x()]);
        //swap columns
        if(firstElement.y() != i) {
            for(unsigned int j = 0; j < sMatrix.size(); j++) iter_swap(sMatrix[j].begin() + i, sMatrix[j].begin() + firstElement.y());
            iter_swap(sColumns.begin() + i, sColumns.begin() + firstElement.y());
        }
        for(unsigned int j = 0; j < sMatrix.size(); j++){
            double multiplier = sMatrix[j][i] / sMatrix[i][i];
            if(j != i) for(unsigned int k = 0; k < sMatrix[j].size(); k++) sMatrix[j][k] -= multiplier * sMatrix[i][k];
        }
    }

    for(unsigned int i = 0; i < sMatrix.size(); i++) {
        sMatrix[i][sMatrix[i].size() - 1] /= sMatrix[i][i];
        sMatrix[i][i] /= sMatrix[i][i];
    }

    sComputed = true;
}

void GaussJordan::compute_interval() {
    for(unsigned int i = 0; i < iMatrix.size(); i++) {
        QPoint firstElement = maxElement(true, i);
        if(iMatrix[firstElement.x()][firstElement.y()].a <= 0 && iMatrix[firstElement.x()][firstElement.y()].b >= 0){
            this->st = 3;
            return;
        }
        //swap rows
        if(firstElement.x() != i) iMatrix[i].swap(iMatrix[firstElement.x()]);
        //swap columns
        if(firstElement.y() != i) {
            for(unsigned int j = 0; j < iMatrix.size(); j++) iter_swap(iMatrix[j].begin() + i, iMatrix[j].begin() + firstElement.y());
            iter_swap(iColumns.begin() + i, iColumns.begin() + firstElement.y());
        }

        for(unsigned int j = 0; j < iMatrix.size(); j++){
            Interval<double> multiplier = iMatrix[j][i] / iMatrix[i][i];
            if(j != i) for(unsigned int k = 0; k < iMatrix[j].size(); k++) iMatrix[j][k] = iMatrix[j][k] - (multiplier * iMatrix[i][k]);
        }
    }

    for(unsigned int i = 0; i < iMatrix.size(); i++) {
        iMatrix[i][iMatrix[i].size() - 1] = iMatrix[i][iMatrix[i].size() - 1] / iMatrix[i][i];
        iMatrix[i][i] = iMatrix[i][i] / iMatrix[i][i];
    }

    iComputed = true;
}

QPoint GaussJordan::maxElement(bool standard, int n) {
    QPoint maxElement(n,n);

    if(standard) {
        for(unsigned int i = n; i < sMatrix.size(); i++){
            for(unsigned int j = n; j < sMatrix[i].size() - 1; j++){
                if(fabs(sMatrix[maxElement.x()][maxElement.y()]) < fabs(sMatrix[i][j])) {
                    maxElement.setX(i);
                    maxElement.setY(j);
                }
            }
        }
    } else {
        for(int i = n; i < iMatrix.size(); i++){
            for(int j = n; j < iMatrix[i].size(); j++){
                if(iMatrix[maxElement.x()][maxElement.y()].b < iMatrix[i][j].a) {
                    maxElement.setX(i);
                    maxElement.setY(j);
                }
            }
        }
    }

    return maxElement;
}
