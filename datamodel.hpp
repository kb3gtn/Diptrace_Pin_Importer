#ifndef DATAMODEL_HPP
#define DATAMODEL_HPP

#include <QAbstractTableModel>
#include <QString>

class DataModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    DataModel( QObject *parent );

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
    //void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

protected:
    std::vector< std::vector<QString> > data_records;
};

#endif // DATAMODEL_HPP
