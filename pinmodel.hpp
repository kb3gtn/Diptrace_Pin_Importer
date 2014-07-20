#ifndef PINMODEL_HPP
#define PINMODEL_HPP

#include <QAbstractTableModel>
#include <vector>
#include <QString>
#include <QStringList>

// simplifing typedefs
typedef std::vector< std::vector<QString> > PinDataRec;
typedef std::vector< std::vector<QString> >::iterator PinDataRecIter;

class PinModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PinModel(QObject *parent = 0);
    // members our table view expects to be here..
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;



    // load data model from CSV
    void load_from_csv( QString filename );
    // save data model to CSV
    void save_to_csv( QString filename );
    // save data model as DipTrace ASC
    void save_to_asc( QString filename );

signals:
    void dataChange(const QModelIndex start, const QModelIndex Stop);

public slots:

protected:
    PinDataRec data_records;
    QStringList get_pin_groups();
    PinDataRec get_pins_in_group(QString group);

};

#endif // PINMODEL_HPP
