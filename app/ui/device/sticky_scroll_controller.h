#pragma once

#include <QObject>
#include <QPointer>
#include <QPersistentModelIndex>

class QAbstractItemModel;
class QTableView;

namespace Ui::Device {

class StickyScrollController : public QObject {
    Q_OBJECT

public:
    explicit StickyScrollController(QTableView* view, QAbstractItemModel* model, QObject* parent = nullptr);

private:
    void prepareForRowsInsertion();
    void followInsertedRows();
    void prepareForRowsRemoval(const QModelIndex& parent, int first, int last);
    void preservePositionAfterRowsRemoval();
    void updateFollowingState();
    void followTailAfterLayout();
    void captureVisibleAnchor();
    void adjustVisibleAnchorForRemoval(int first, int last);
    void restorePositionAfterLayout();
    bool isAtBottom() const;

private:
    QPointer<QTableView> m_view;
    QPointer<QAbstractItemModel> m_model;
    QPersistentModelIndex m_visibleAnchor;
    int m_visibleAnchorOffset = 0;
    bool m_followingTail = true;
    bool m_modelChanging = false;
    bool m_tailFollowPending = false;
    bool m_positionRestorePending = false;
};

}  // namespace Ui::Device
