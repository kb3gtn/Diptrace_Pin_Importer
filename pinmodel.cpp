#include "pinmodel.hpp"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <sstream>
#include <QMessageBox>
#include <iostream>

PinModel::PinModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    // create a initial data model..
    std::vector<QString> entry;
    entry.push_back(QString("No"));
    entry.push_back(QString("Data"));
    entry.push_back(QString("Available"));
    data_records.push_back(entry);
}

// members our table view expects to be here..
int PinModel::rowCount(const QModelIndex &parent) const
{
    // return the number of records we have
    return data_records.size();
}

int PinModel::columnCount(const QModelIndex &parent) const
{
    // return the number of entries per record we are tracking.
    // uses first record has our reference.
    return data_records[0].size();
}

QVariant PinModel::data(const QModelIndex &index, int role) const
{
    if ( role == Qt::DisplayRole)
    {
        return data_records[index.row()][index.column()];
    }
    // else invalid query
    return QVariant::Invalid;
}

// called by the mode view widget (tableview) to update it's header information
QVariant PinModel::headerData(int section, Qt::Orientation orientation, int role) const
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
void PinModel::load_from_csv( QString filename )
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
    QModelIndex BottomRight = createIndex(data_records.size(),3);
    std::cout << "Updated tableView to new data size" << std::endl;
    emit dataChanged(topLeft, BottomRight);
}

// save data model to CSV
void PinModel::save_to_csv( QString filename )
{
    QFile file( filename );
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(0, "error", file.errorString());
        return;
    }

    QTextStream out(&file);
    int i;
    out << "Pin Name, Pin Number, Pin Group\n";
    for ( i=0; i < data_records.size(); i++ ) {
        out << data_records[i][0] << "," << data_records[i][1] << ","\
                                  << data_records[i][2] << "\n";
    }

    std::cout << "File write complete.." << std::endl;
}

QStringList PinModel::get_pin_groups()
{
    QStringList pin_groups;
    PinDataRecIter pin_iter;
    // iterate though complete pin database..
    for ( pin_iter = data_records.begin();
          pin_iter != data_records.end();
          pin_iter++) {
        // add column 3 (from each record) to pin list..
        pin_groups.append( (*pin_iter)[2] );
    }

    std::cout << "Pulled data records for pin groups, removing dups.." << std::endl;


    //remove dups
    pin_groups.removeDuplicates();

    return pin_groups;

}

// go though pin data base and pull pins from selected group.
PinDataRec PinModel::get_pins_in_group(QString group)
{
    PinDataRec pin_list;
    PinDataRecIter master_iter;

    // simple linear search though master pin list
    for (master_iter = data_records.begin();
         master_iter != data_records.end();
         ++master_iter ) {
        if ( (*master_iter)[2] == group ) {
            // got a hit, add to pin_list
            pin_list.push_back((*master_iter));
        }
    }

    return pin_list;
}

// save data model as DipTrace ASC
void PinModel::save_to_asc( QString filename )
{
    QFile file( filename );
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::information(0, "error", file.errorString());
        return;
    }


    QTextStream out(&file);

    std::cout << "Starting build of library component." << std::endl;

    // DIP trace libary header crap
    out << "(Source \"DipTrace-ElementLibrary\")\n";
    out << "  (Library\n";
    out << "    (Name \"Generated Part\")\n";
    out << "    (Hint \"mported Part\")\n";
    out << "    (Subfolders\n";
    out << "    )\n";
    out << "    (Components\n";
    out << "      (Component\n";

    std::cout << "Generating list of pin groups...." << std::endl;

    // Now figure out how many pin groups we have.
    QStringList pin_groups = get_pin_groups();
    QStringList::const_iterator pin_group_iter;

    std::cout << "Detected " << pin_groups.size() << " different pin groups." << std::endl;

    // loop over pin groups and create parts
    for ( pin_group_iter = pin_groups.constBegin();
          pin_group_iter != pin_groups.constEnd();
          ++pin_group_iter) {
        std::cout << "Working on Pin Group " << (*pin_group_iter).toStdString() << "." << std::endl;
        out << "        (Part \"Part " << (*pin_group_iter).toLocal8Bit().constData() << "\" \"\"\n";
        out << "          (Enabled \"Y\")\n";
        out << "          (PartType 0)\n";
        out << "          (PartName \"PART " << (*pin_group_iter).toLocal8Bit().constData() << "\")\n";
        out << "          (ShowNumbers 0)\n";
        out << "          (Type 0)\n";
        out << "          (Number1 0)\n";
        out << "          (Number2 0)\n";
        out << "          (Width 0)\n";
        out << "          (Height 0)\n";
        out << "          (Value \"\")\n";
        out << "          (LockProperties \"N\")\n";
        out << "          (OriginX 0)\n";
        out << "          (OriginY 0)\n";
        out << "          (Datasheet \"\")\n";
        out << "          (ModelType 0)\n";
        out << "          (ModelString \"\")\n";
        out << "          (ModelBody\n";
        out << "          )\n";
        out << "          (SubfolderIndex -1)\n";
        out << "          (Verification \"N\" \"N\" \"N\" \"N\" \"N\" \"N\" \"N\")\n";
        out << "          (Pins\n";

        // now loop though all the pins in this group
        PinDataRec pins_in_group = get_pins_in_group( (*pin_group_iter) );
        PinDataRecIter pin_iter;
        double y = 0;  // pin location in mm
        int pin_num = 0; // pin nubmer in this part.. (counter)
        for ( pin_iter = pins_in_group.begin();
              pin_iter != pins_in_group.end();
              ++pin_iter) {
            out << "            (Pin " << pin_num << " 0 " << y << "\n";
            pin_num++;
            y = y - 7.62;  // compute pin spacing (in mm  2.54mm -> 0.1 inch)
            out << "              (Enabled \"Y\")\n";
            out << "              (Locked \"N\")\n";
            out << "              (ModelSig \"\")\n";
            out << "              (Type 0)\n";
            out << "              (Orientation 2)\n";
            out << "              (Number " << pin_num << ")\n";
            out << "              (Length 11.43)\n";
            out << "              (Name \"" << (*pin_iter)[0] << "\")\n";
            out << "              (StringNumber \"" << (*pin_iter)[1] << "\")\n";
            out << "              (ShowName \"Y\")\n";
            out << "              (PinNumXShift 0)\n";
            out << "              (PinNumYShift 0)\n";
            out << "              (PinNamexShift 0)\n";
            out << "              (PinNameYShift 0)\n";
            out << "              (ElectricType 0)\n";
            out << "              (NameFontSize 5)\n";
            out << "              (NameFontWidth -2)\n";
            out << "              (NameFontScale 1)\n";
            out << "            )\n";
        }

        // close out part
        out << "          )\n";
        out << "        )\n";

        // ready for next pin group

    }

    // close out file..
    out << "      )\n";
    out << "    )\n";
    out << "  )\n";
    out << ")\n";

    out.flush();

    std::cout << "Export Complete.." << std::endl;
}
