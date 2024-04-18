#ifndef FILESVIEW_H
#define FILESVIEW_H

#include "../FilesModel.h"

class FilesView : public QTableView
{
    Q_OBJECT

public:

    explicit FilesView(QWidget* parent = nullptr);
    virtual ~FilesView() override;

private:

    //
    //
    QMenu* m_pContextMenu = nullptr;

    QAction* m_pEncodeAction = nullptr;
    QAction* m_pDecodeAction = nullptr;

    QMenu* m_headerContextMenu = nullptr;
    std::vector<QAction*> m_pColumnActionList;

    //
    //
    FilesModel m_model;

    void createContextMenu();

    void createHeaderContextMenu();
    void hideListColumn(int column, bool hide);
    int firstVisibleColumn();

protected:

    bool eventFilter(QObject* object, QEvent* e) override;

signals:

    void fileSelected(const FileItem& item);
    void fileEncode();
    void fileDecode();

public slots:

    //
    //
    void onFilesLoaded(const std::vector<FileItem>& list_add);

    void onEncode() { emit fileEncode(); }
    void onDecode() { emit fileDecode(); }

    // slots of menu
    //
    void onContextMenu(const QPoint&);

    // Slot of Click
    //
    void onListCliked(const QModelIndex& index);

    // slots for list header, to hide or show columns
    //
    void onHeaderContextMenu(QPoint);
    void onColumnAction(QAction* action);
};

#endif // FILESVIEW_H
