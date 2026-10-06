#include "app/ui/device/sticky_scroll_controller.h"

#include <QAbstractItemModel>
#include <QPoint>
#include <QScrollBar>
#include <QTableView>
#include <QTimer>

namespace Ui::Device {

StickyScrollController::StickyScrollController(
    QTableView* view,
    QAbstractItemModel* model,
    QObject* parent)
    : QObject(parent)
    , m_view(view)
    , m_model(model)
{
    if (!m_view || !m_model) {
        return;
    }

    connect(
        m_model,
        &QAbstractItemModel::rowsAboutToBeInserted,
        this,
        &StickyScrollController::prepareForRowsInsertion);
    connect(
        m_model,
        &QAbstractItemModel::rowsInserted,
        this,
        &StickyScrollController::followInsertedRows);
    connect(
        m_model,
        &QAbstractItemModel::rowsAboutToBeRemoved,
        this,
        &StickyScrollController::prepareForRowsRemoval);
    connect(
        m_model,
        &QAbstractItemModel::rowsRemoved,
        this,
        &StickyScrollController::preservePositionAfterRowsRemoval);
    connect(
        m_view->verticalScrollBar(),
        &QScrollBar::valueChanged,
        this,
        &StickyScrollController::updateFollowingState);
}

void StickyScrollController::prepareForRowsInsertion()
{
    if (!m_view) {
        return;
    }

    if (!m_tailFollowPending) {
        m_followingTail = isAtBottom();
    }

    m_modelChanging = true;
    if (!m_followingTail && !m_positionRestorePending) {
        captureVisibleAnchor();
    }
}

void StickyScrollController::followInsertedRows()
{
    if (!m_view) {
        return;
    }

    if (m_followingTail) {
        followTailAfterLayout();
    } else {
        restorePositionAfterLayout();
    }

    m_modelChanging = false;
}

void StickyScrollController::prepareForRowsRemoval(const QModelIndex& parent, int first, int last)
{
    if (!m_view || parent.isValid()) {
        return;
    }

    if (!m_tailFollowPending && !m_positionRestorePending) {
        m_followingTail = isAtBottom();
    }

    m_modelChanging = true;

    if (m_followingTail || !m_model) {
        return;
    }

    if (!m_positionRestorePending) {
        captureVisibleAnchor();
    }

    adjustVisibleAnchorForRemoval(first, last);
}

void StickyScrollController::preservePositionAfterRowsRemoval()
{
    if (!m_view) {
        return;
    }

    if (m_followingTail) {
        followTailAfterLayout();
    } else {
        restorePositionAfterLayout();
    }

    m_modelChanging = false;
}

void StickyScrollController::updateFollowingState()
{
    if (m_modelChanging || m_tailFollowPending || m_positionRestorePending) {
        return;
    }

    m_followingTail = isAtBottom();
}

void StickyScrollController::followTailAfterLayout()
{
    if (m_tailFollowPending) {
        return;
    }

    m_tailFollowPending = true;
    QTimer::singleShot(
        0,
        this,
        [this]() {
            if (m_view && m_followingTail) {
                m_view->scrollToBottom();
            }

            m_tailFollowPending = false;
        }
    );
}

void StickyScrollController::captureVisibleAnchor()
{
    m_visibleAnchor = QPersistentModelIndex();
    m_visibleAnchorOffset = 0;
    if (!m_view) {
        return;
    }

    const QModelIndex anchor = m_view->indexAt(QPoint(0, 0));
    if (!anchor.isValid()) {
        return;
    }

    m_visibleAnchor = anchor;
    m_visibleAnchorOffset = m_view->visualRect(anchor).top();
}

void StickyScrollController::adjustVisibleAnchorForRemoval(int first, int last)
{
    if (!m_model || !m_visibleAnchor.isValid()
        || m_visibleAnchor.row() < first || m_visibleAnchor.row() > last) {
        return;
    }

    const int column = m_visibleAnchor.column();
    const int nextSurvivingRow = last + 1;
    if (nextSurvivingRow < m_model->rowCount()) {
        m_visibleAnchor = m_model->index(nextSurvivingRow, column);
    } else if (first > 0) {
        m_visibleAnchor = m_model->index(first - 1, column);
    } else {
        m_visibleAnchor = QPersistentModelIndex();
    }
}

void StickyScrollController::restorePositionAfterLayout()
{
    if (m_positionRestorePending) {
        return;
    }

    m_positionRestorePending = true;
    QTimer::singleShot(
        0,
        this,
        [this]() {
            if (m_view) {
                m_view->doItemsLayout();
                if (m_visibleAnchor.isValid()) {
                    m_view->scrollTo(m_visibleAnchor, QAbstractItemView::PositionAtTop);
                    QScrollBar* scrollBar = m_view->verticalScrollBar();
                    scrollBar->setValue(scrollBar->value() - m_visibleAnchorOffset);
                } else {
                    m_view->scrollToTop();
                }
            }

            m_visibleAnchor = QPersistentModelIndex();
            m_visibleAnchorOffset = 0;
            m_positionRestorePending = false;
        }
    );
}

bool StickyScrollController::isAtBottom() const
{
    if (!m_view) {
        return true;
    }

    const QScrollBar* scrollBar = m_view->verticalScrollBar();
    return scrollBar->value() >= scrollBar->maximum();
}

}  // namespace Ui::Device
