#include "ISelectable.h"

void ISelectable::SetSelectionState(SelectionState const state)
{
    if (state == m_selectionState)
    {
        return;
    }
    m_selectionState = state;
    InvalidatePaintArea();
}
