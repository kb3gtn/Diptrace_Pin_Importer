#include "datamodel.hpp"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <sstream>
#include <QMessageBox>
#include <iostream>

DataModel::DataModel(QObject *parent) : QAbstractTableModel(parent)
{
    // create a initial data model..
    std::vector<QString> entry;
    entry.push_back(QString("No"));
    entry.push_back(QString("Data"));
    entry.push_back(QString("Available"));
    data_records.push_back(entry);
}

// members our table view expects to be here..
int DataModel::rowCount(const QModelIndex &parent) const
{
    // return the number of records we have
    return data_records.size();
}

int DataModel::columnCount(const QModelIndex &parent) const
{
    // return the number of entries per record we are tracking.
    // uses first record has our reference.
    return data_records[0].size();
}

QVariant DataModel::data(const QModelIndex &index, int role) const
{
    if ( role == Qt::DisplayRole)
    {
        return data_records[index.row()][index.column()];
    }
    // else invalid query
    return QVariant::Invalid;
}

// called by the mode view widget (tableview) to update it's header information
QVariant DataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        std::stringstream ss;
        if (orientation == Qt::Horizontal)
        {
            //ss << "H_" << section;
            if ( section == 0 ) {
                return QString("Pin Name");
            }
            if ( section == 1 ) {
                return QString("Pin Number");
            }
            if ( section == 2 ) {
                return QString("Pin Group");
            }
            // else
            return QString("???");
        }
        else if ( orientation == Qt::Vertical)
        {
            ss << section;  // just a number of the record
            QString val = QString::fromStdString(ss.str());
            return val;
        }
    }

    // got here.. queried with invalid inputs
    return QVariant::Invalid;
}

// load data model from CSV
void DataModel::load_from_csv( QString filename )
{
    // Load from CSV, we expect 3 columns of data per row.
    // <Pin Name> <Pin Number> <Sub Part #>
    // basically pins can be broken up into groups of sub parts.

    // Try to opena csv file source.
    QFile file( filename );
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
        return;
    }

    // File is now open...

    // Flush currently model data set
    data_records.clear();

    // Use a QTextStream object to read the file data
    QTextStream in(&file);
    // first line in file is assumed to be headers. (skip it)
    in.readLine();

    while (!in.atEnd()) {
        QString line = in.readLine();
        // we assume simple CSV with no quotes or ',' in any feilds.
        QStringList fields = line.split(",");
        if ( fields.size() == 3 )
        {
            // add entry to out data modle
            std::vector<QString> entry;
            entry.push_back(fields[0].replace(" ", ""));  // pin name  (remove white spaces)
            entry.push_back(fields[1]);  // pin number
            entry.push_back(fields[2]);  // pin group
            data_records.push_back(entry); // add entry to data_records vector.
            // debug what what loaded..
            //std::cout << "Read: [ " << fields[0].toStdString() << ", " \
            //          << fields[1].toStdString() << ", " \
            //          << fields[2].toStdString() << " ]" << std::endl;
        } else {
            // print out any entries we are rejecting..
            std::cout << "Rejected [ " << line.toStdString() << " ]  Bad Format.. " << std::endl;
        }
    }

    // signal model view data was updated..
    QModelIndex topLeft = createIndex(0,0);
    //emit dataChanged(topLeft, topLeft);

}

// save data model to CSV
void DataModel::save_to_csv( QString filename )
{

}

// save data model as DipTrace ASC
void DataModel::save_to_asc( QString filename )
{

}
