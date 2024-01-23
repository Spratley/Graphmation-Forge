#include "GraphmationForgeApp.h"

#include "GraphmationColors.h"
#include "Node.h"
#include "Transition.h"
#include "StringConvert.h"

#include "JParse/JParse.h"
#include "ParsingDefines.h"

#include "resource.h"

#include <windowsx.h>

GraphmationForgeApp* GraphmationForgeApp::s_instance;

// Global callback for Win32
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    GraphmationForgeApp* instance = GraphmationForgeApp::GetInstance();
    int retValue = -1;
    switch (message)
    {
    case WM_SHOWWINDOW: // HACK, postpone message to on window show
        retValue = instance->OnWindowCreated(hWnd, message, wParam, lParam);
        break;
    case WM_ERASEBKGND:
        retValue = instance->OnPaintCustomBackground(hWnd, message, wParam, lParam);
        break;
    case WM_PAINT:
        retValue = instance->OnWindowPaint(hWnd, message, wParam, lParam);
        break;
    case WM_COMMAND:
        retValue = instance->OnWindowCommand(hWnd, message, wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
        retValue = instance->OnLeftMouseButtonDown(hWnd, message, wParam, lParam);
        break;
    case WM_LBUTTONUP:
        retValue = instance->OnLeftMouseButtonUp(hWnd, message, wParam, lParam);
        break;
    case WM_MOUSEMOVE:
        retValue = instance->OnMouseMoved(hWnd, message, wParam, lParam);
        break;
    case WM_CONTEXTMENU:
        retValue = instance->OnOpenContextMenu(hWnd, message, wParam, lParam);
        break;
    case WM_SIZE:
        retValue = instance->OnWindowResize(hWnd, message, wParam, lParam);
        break;
    case WM_CLOSE:
        retValue = instance->OnWindowClosed(hWnd, message, wParam, lParam);
        break;
    case WM_KEYDOWN:
        retValue = instance->OnKeyDown(hWnd, message, wParam, lParam);
        break;
    }

    if (retValue == -1)
    {
        // Window refused to process command, fallback to default behavior
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

GraphmationForgeApp::GraphmationForgeApp()
: m_createTransitionFakeTransition(NULL)
{
    // Don't care about multiple instances because if there is then you already messed up
    s_instance = this;

    GraphmationTimer::Init(); 
    AfxWinInit(m_instanceHandle, NULL, GetCommandLine(), 0);
}

int GraphmationForgeApp::OnWindowCreated(WIN32_CALLBACK_PARAMS)
{
    if (hWnd == m_mainWindowHandle)
    {
        // We're creating the main window
        OnMainWindowCreated(hWnd, message, wParam, lParam);
        return 0;
    }

    // int id = GetWindowLong(hWnd, GWL_ID);
    // switch (id)
    // {
    // default:
    //     return -1;
    // }

    return 0;
}

int GraphmationForgeApp::OnPaintCustomBackground(WIN32_CALLBACK_PARAMS)
{
    int id = GetWindowLong(hWnd, GWL_ID);
    switch (id)
    {
    case ID_CLASS_NODE:
        return PaintNodeBackground(hWnd, message, wParam, lParam);
    default:
        return -1;
    }

    return 0;
}

int GraphmationForgeApp::OnWindowPaint(WIN32_CALLBACK_PARAMS)
{
    if (hWnd == m_mainWindowHandle)
    {
        PaintMainWindow(hWnd, message, wParam, lParam);
        return 0;
    }

    int id = GetWindowLong(hWnd, GWL_ID);
    switch (id)
    {
    case ID_CLASS_NODE:
        PaintNode(hWnd, message, wParam, lParam);
        break;
    case ID_CLASS_PROPERTIES_PANEL:
        m_propertiesWindow.Paint(hWnd, message, wParam, lParam);
    default:
        return -1;
    }

    return 0;
}

int GraphmationForgeApp::OnWindowCommand(WIN32_CALLBACK_PARAMS)
{
    int commandID = LOWORD(wParam);

    // Special case for delete condition button
    if (PropertiesWindow::IsDeleteConditionButton(commandID))
    {
        m_propertiesWindow.DeleteCondition(commandID - ID_COMMAND_DELETE_CONDITION);
        m_propertiesWindow.RebuildPropertiesContent();
        return 0;
    }

    // Parse the menu selections:
    switch (commandID)
    {
    case IDM_EXIT:
        if (TryUnloadGraph())
        {
            DestroyWindow(hWnd);
        }
        break;
    case IDM_NEW:
        if (TryUnloadGraph())
        {
            RECT mainWindowRect;
            GetClientRect(m_mainWindowHandle, &mainWindowRect);
            InvalidateRect(m_mainWindowHandle, &mainWindowRect, true);
        }
        break;
    case IDM_OPEN:
        OpenFile();
        break;
    case IDM_SAVE:
        SaveFile();
        break;
    case IDM_SAVE_AS:
        SaveAsFile();
        break;
    case IDM_INSERT_ANIMSTATE:
        CreateNode();
        break;
    case ID_CONTEXT_CREATE_ANIMSTATE:
        CreateNodeAtMousePos();
        break;
    case ID_CONTEXT_CREATE_TRANSITION:
        BeginCreateTransition();
        break;
    case ID_COMMAND_EDIT:
        if (HIWORD(wParam) == EN_UPDATE)
        {
            m_propertiesWindow.PropagatePropertyValues();
        }
        break;
    case ID_COMMAND_DROPDOWN:
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            m_propertiesWindow.PropagatePropertyValues();
           // m_propertiesWindow.RebuildPropertiesContent(); // For some reason this causes a crash
        }
    case ID_COMMAND_NEW_CONDITION:
        if (HIWORD(wParam) == BN_CLICKED)
        {
            if (m_selectedObjects.size() == 1)
            {
                if (Transition* transition = dynamic_cast<Transition*>(*m_selectedObjects.begin()))
                {
                    transition->AddNewCondition();
                    m_propertiesWindow.RebuildPropertiesContent();
                }
            }
        }
    default:
        return -1;
    }
    return 0;
}

int GraphmationForgeApp::OnLeftMouseButtonDown(WIN32_CALLBACK_PARAMS)
{   
    // Custom behavior for creating transition
    if (m_currentAppState == Create_Transition)
    {
        if (m_potentialSelectable)
        {
            if (Node* selectedNode = dynamic_cast<Node*>(m_potentialSelectable))
            {
                CreateTransition(m_createTransitionStartingNode, selectedNode);
            }
        }

        m_currentAppState = Default;
        m_createTransitionFakeTransition.ForceInvalidateCurrentPaintRegion(); // Redraw fake transition to clear it off the screen

        return 0;
    }






    // if (hWnd != m_mainWindowHandle)
    // {
    //     // Only interpret mouse button down for the main window
    //     return -1;
    // }

    if (hWnd == m_propertiesWindow.GetHWND())
    {
        // Don't interpret clicks on the properties panel
        return -1;
    }

    if (!m_potentialSelectable)
    {
        DeselectAll();
        return 0;
    }

    POINT mousePos;
    GetCursorPos(&mousePos);
    ScreenToClient(m_mainWindowHandle, &mousePos);

    m_isDragging = false;

    bool hasSelectedNodes = m_selectedObjects.size();
    if (hasSelectedNodes && DragDetect(hWnd, mousePos))
    {
        for (ISelectable* selectable : m_selectedObjects)
        {
            if (Node* node = dynamic_cast<Node*>(selectable))
            {
                node->StartDrag(mousePos);
            }
        }

        m_isDragging = true;
    }
    else
    {
        // Left click on selectable
        bool isShiftHeld = wParam & MK_SHIFT;
        if (isShiftHeld)
        {
            // Just toggle selection of object
            if (IsSelected(m_potentialSelectable))
            {
                Deselect(m_potentialSelectable);
            }
            else
            {
                Select(m_potentialSelectable);
            }
        }
        else
        {
            // If we are selected and there are no other selected objects, deselect self.
            // If we are selected and there ARE other selected objects, select only self.
            // If we are deselected, select only self

            int startingSelectionSize = m_selectedObjects.size();
            bool potentialSelectableAlreadySelected = IsSelected(m_potentialSelectable);
            DeselectAll();
            if (!potentialSelectableAlreadySelected || (potentialSelectableAlreadySelected && startingSelectionSize > 1))
            {
                Select(m_potentialSelectable);
            }
        }
    }

    return 0;
}

int GraphmationForgeApp::OnLeftMouseButtonUp(WIN32_CALLBACK_PARAMS)
{
    if (m_isDragging)
    {
        m_containsUnsavedChanges = true;
    }

    m_isDragging = false;
    return 0;
}

int GraphmationForgeApp::OnMouseMoved(WIN32_CALLBACK_PARAMS)
{
    POINT mousePos;
    GetCursorPos(&mousePos);
    ScreenToClient(m_mainWindowHandle, &mousePos);

    if (m_isDragging)
    {
        // Check if enough time has passed to poll drag update
        TimePoint timeSinceUpdate = GraphmationTimer::GetCurrentTimeMS() - m_lastUpdateTime;
        if (timeSinceUpdate < 80)
        {
            return -1;
        }
        m_lastUpdateTime = GraphmationTimer::GetCurrentTimeMS();

        InvalidateAttachedTransitions(m_selectedObjects);
        for (ISelectable* selectable : m_selectedObjects)
        {
            Node* node = dynamic_cast<Node*>(selectable);
            if (node)
            {
                node->SetDragged(mousePos);
            }
        }
        InvalidateAttachedTransitions(m_selectedObjects);
    }

    m_potentialSelectable = nullptr;
    // Check transitions
    for (Transition* const transition : m_transitions)
    {
        SelectionState nextMouseOverState = NONE;
        if (transition->IsMouseOverlapping(mousePos))
        {
            m_potentialSelectable = transition;
            nextMouseOverState = HIGHLIGHTED;
        }

        if (transition->GetSelectionState() != SELECTED)
        {
            transition->SetSelectionState(nextMouseOverState);
        }
    }

    // Check nodes after so they get priority for selection
    for (Node* const node : m_nodes)
    {
        SelectionState nextMouseOverState = NONE;
        if (node->IsMouseOverlapping(mousePos))
        {
            m_potentialSelectable = node;
            nextMouseOverState = HIGHLIGHTED;
        }

        if (node->GetSelectionState() != SELECTED)
        {
            node->SetSelectionState(nextMouseOverState);
        }
    }

    // If creating transition, redraw arrow that goes to mouse position
    if (m_currentAppState == Create_Transition)
    {
        TimePoint timeSinceUpdate = GraphmationTimer::GetCurrentTimeMS() - m_lastUpdateTime;
        if (timeSinceUpdate < 80)
        {
            return -1;
        }
        m_lastUpdateTime = GraphmationTimer::GetCurrentTimeMS();

        // HACK: Just draw the arrow because coding is hard :(
        m_createTransitionFakeTransition.ForceInvalidateCurrentPaintRegion();
        m_createTransitionFakeTransition.SetToPoint(mousePos);
        m_createTransitionFakeTransition.InvalidatePaintArea();
        
        // Lets try invalidating the entire client rect
        // RECT mainWindowRect;
        // GetClientRect(m_mainWindowHandle, &mainWindowRect);
        // InvalidateRect(m_mainWindowHandle, &mainWindowRect, true);
    }


    return 0;
}

int GraphmationForgeApp::OnOpenContextMenu(WIN32_CALLBACK_PARAMS)
{
    if (m_currentAppState != Default)
    {
        return -1;
    }

    if (hWnd == m_mainWindowHandle)
    {
        POINT mousePos;
        GetCursorPos(&mousePos);

        HMENU hPopupMenu = CreatePopupMenu();
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_CONTEXT_CREATE_ANIMSTATE, L"Insert Animation State");
        SetForegroundWindow(hWnd);
        TrackPopupMenu(hPopupMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, mousePos.x, mousePos.y, 0, hWnd, NULL);
        return 0;
    }

    int const id = GetWindowLong(hWnd, GWL_ID);
    if (id == ID_CLASS_NODE)
    {
        POINT mousePos;
        GetCursorPos(&mousePos);

        HMENU hPopupMenu = CreatePopupMenu();
        InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, ID_CONTEXT_CREATE_TRANSITION, L"Create Transition");
        SetForegroundWindow(hWnd);
        TrackPopupMenu(hPopupMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, mousePos.x, mousePos.y, 0, hWnd, NULL);
        return 0;
    }
    return -1;
}

int GraphmationForgeApp::OnWindowResize(WIN32_CALLBACK_PARAMS)
{
    if (hWnd == m_mainWindowHandle)
    {
        m_propertiesWindow.UpdatePropertiesWindowSize();
    }

    return 0;
}

int GraphmationForgeApp::OnWindowClosed(WIN32_CALLBACK_PARAMS)
{
    if (hWnd != m_mainWindowHandle)
    {
        return -1;
    }

    if (TryUnloadGraph())
    {
        PostQuitMessage(0);
        return 0;
    }
    return 1;
}

int GraphmationForgeApp::OnKeyDown(WIN32_CALLBACK_PARAMS)
{
    if (wParam == VK_BACK || wParam == VK_DELETE)
    {
        DeleteSelected();
    }

    return 0;
}

ATOM GraphmationForgeApp::RegisterWindowClass(LPCWSTR className, HBRUSH backgroundBrush)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_instanceHandle;
    wcex.hIcon = LoadIcon(m_instanceHandle, MAKEINTRESOURCE(IDI_GRAPHMATIONFORGE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = backgroundBrush;
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GRAPHMATIONFORGE);
    wcex.lpszClassName = className;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

void GraphmationForgeApp::DeleteSelected()
{
    for (ISelectable* selected : m_selectedObjects)
    {
        if (Node* nodeSelectable = dynamic_cast<Node*>(selected))
        {
            for (int i = 0; i < m_nodes.size(); i++)
            {
                if (nodeSelectable == m_nodes[i])
                {
                    delete m_nodes[i];
                    m_nodes.erase(m_nodes.begin() + i);
                    break;
                }
            }
        }
        else if (Transition* transitionSelectable = dynamic_cast<Transition*>(selected))
        {
            for (int i = 0; i < m_transitions.size(); i++)
            {
                if (transitionSelectable == m_transitions[i])
                {
                    delete m_transitions[i];
                    m_transitions.erase(m_transitions.begin() + i);
                    break;
                }
            }
        }
    }

    RECT mainWindowRect;
    GetClientRect(m_mainWindowHandle, &mainWindowRect);
    InvalidateRect(m_mainWindowHandle, &mainWindowRect, true);

    m_selectedObjects.clear();
    m_propertiesWindow.ClearPropertiesContent();
}

void GraphmationForgeApp::OnMainWindowCreated(WIN32_CALLBACK_PARAMS)
{
    // Create all default sub-windows
    HWND propertiesWindowHandle =
        CreateWindow(m_stringResources[ID_CLASS_PROPERTIES_PANEL],
            L"Properties",
            WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            m_mainWindowHandle,
            (HMENU)ID_CLASS_PROPERTIES_PANEL, NULL, NULL);

    m_propertiesWindow = PropertiesWindow(propertiesWindowHandle, m_mainWindowHandle);
    m_createTransitionFakeTransition = FakeTransition(m_mainWindowHandle);
}

void GraphmationForgeApp::PaintMainWindow(WIN32_CALLBACK_PARAMS)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    for (Transition* transition : m_transitions)
    {
        ColorIDs color = ID_COLOR_TRANSITION_DESELECTED;
        SelectionState const transitionState = transition->GetSelectionState();
        if (transitionState == HIGHLIGHTED)
        {
            color = ID_COLOR_TRANSITION_HIGHLIGHTED;
        }
        else if (transitionState == SELECTED)
        {
            color = ID_COLOR_TRANSITION_SELECTED;
        }

        transition->Paint(hdc, m_brushes[color]);
    }

    if (m_currentAppState == Create_Transition)
    {
        m_createTransitionFakeTransition.Paint(hdc, m_brushes[ID_COLOR_TRANSITION_DESELECTED]);
    }

    EndPaint(hWnd, &ps);
}

void GraphmationForgeApp::PaintNode(WIN32_CALLBACK_PARAMS)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    
    Node* nodeToPaint = FindNode(hWnd);

    RECT textArea;
    textArea.left = 0;
    textArea.top = 0;
    textArea.right = 150;
    textArea.bottom = 50;
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, g_colors[ID_COLOR_FONT]);
    SelectObject(hdc, m_fonts[0]);

    std::wstring const& nodeName = nodeToPaint->GetNodeName();
    DrawText(hdc, nodeName.c_str(), nodeName.size(), &textArea, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        
    EndPaint(hWnd, &ps);
}

int GraphmationForgeApp::PaintNodeBackground(WIN32_CALLBACK_PARAMS)
{
    Node* nodeToPaint = FindNode(hWnd);
    // Do nothing if we are deselected, default background is OK
    HBRUSH backgroundColor;
    switch (nodeToPaint->GetSelectionState())
    {
    default:
    case NONE:
        return -1;
    case HIGHLIGHTED:
        backgroundColor = m_brushes[ID_COLOR_NODE_HIGHLIGHTED];
        break;
    case SELECTED:
        backgroundColor = m_brushes[ID_COLOR_NODE_SELECTED];
    }

    HDC hdc = (HDC)wParam;
    RECT nodeRect;
    GetClientRect(hWnd, &nodeRect);
    FillRect(hdc, &nodeRect, backgroundColor);
    return 0;
}

Node * const GraphmationForgeApp::FindNode(HWND const nodeWindowHandle) const
{
    for (Node* const node : m_nodes)
    {
        if (node->GetWindowHandle() == nodeWindowHandle)
        {
            return node;
        }
    }
    return nullptr;
}

void GraphmationForgeApp::Select(ISelectable * const selection)
{
    if (IsSelected(selection))
    {
        return;
    }

    selection->SetSelectionState(SELECTED);
    m_selectedObjects.insert(selection);
    OnSelectionChanged();
}

bool const GraphmationForgeApp::Deselect(ISelectable * const selection)
{
    if (!IsSelected(selection))
    {
        return false;
    }

    selection->SetSelectionState(NONE);
    m_selectedObjects.erase(selection);
    OnSelectionChanged();
    return true;
}

void GraphmationForgeApp::DeselectAll()
{
    for (ISelectable* selectable : m_selectedObjects)
    {
        selectable->SetSelectionState(NONE);
    }
    m_selectedObjects.clear();

    OnSelectionChanged();
}

bool const GraphmationForgeApp::IsSelected(ISelectable * const selection)
{
    return m_selectedObjects.count(selection);
}

void GraphmationForgeApp::OnSelectionChanged()
{
    if (m_selectedObjects.size() == 1)
    {
        // Use begin() to get the address of the "first" and only element in the set
        m_propertiesWindow.SetPropertiesContent(*m_selectedObjects.begin());
        return;
    }
    m_propertiesWindow.ClearPropertiesContent();
}

void GraphmationForgeApp::BeginCreateTransition()
{
    if (!m_potentialSelectable)
    {
        return;
    }

    Node* startingNode = dynamic_cast<Node*>(m_potentialSelectable);
    if (!startingNode)
    {
        return;
    }

    m_createTransitionStartingNode = startingNode;
    POINT fromPoint = startingNode->GetNodePosition();
    fromPoint.x += NODE_MIN_WIDTH / 2;
    fromPoint.y += NODE_HEIGHT / 2;
    m_createTransitionFakeTransition.SetFromPoint(fromPoint);
    m_currentAppState = Create_Transition;
}

bool GraphmationForgeApp::OpenFile()
{
    LPCTSTR filter = L"Animation Graph (*.animgraph)|*.animgraph|";

    CFileDialog dlgFile(TRUE, _T("animgraph"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, filter, CWnd::FromHandle(m_mainWindowHandle));

    if (dlgFile.DoModal() != IDOK)
    {
        return false;
    }

    CString pathCStr(dlgFile.GetPathName().GetString());
    std::wstring path(pathCStr);

    if (path == L"")
    {
        return false;
    }

    if (!TryUnloadGraph())
    {
        return false;
    }

    return LoadJSON(StringConvert::ToStr(path));
}

bool GraphmationForgeApp::SaveFile()
{
    if (m_loadedFilepath == "")
    {
        return SaveAsFile();
    }

    SaveJSON(m_loadedFilepath);
    return false;
}

bool GraphmationForgeApp::SaveAsFile()
{
    LPCTSTR filter = L"Animation Graph (*.animgraph)|*.animgraph|";
    CFileDialog dlgFile(FALSE, _T("animgraph"), L"New Animgraph", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, CWnd::FromHandle(m_mainWindowHandle));

    if (dlgFile.DoModal() != IDOK)
    {
        return false;
    }

    CString pathCStr(dlgFile.GetPathName().GetString());
    std::wstring path(pathCStr);

    m_loadedFilepath = StringConvert::ToStr(path);
    return SaveJSON(m_loadedFilepath);
}

bool GraphmationForgeApp::LoadJSON(std::string filepath)
{
    JParse::Root json;
    json.Parse(filepath);

    JParse::Object* objectRoot = json.m_item->GetAs<JParse::Object>();

    // Load states
    JParse::Array* states = objectRoot->Get<JParse::Array>(STATES_ROOT);
    for (JParse::Item* state : states->m_contents)
    {
        JParse::Object* stateObject = state->GetAs<JParse::Object>();
        // Load state data
        std::string nodeName = TRY_PARSE(stateObject, STATE_NAME, JParse::String, "New State");
        std::string animName = TRY_PARSE(stateObject, STATE_ANIM_NAME, JParse::String, "NULL");
        bool loop = TRY_PARSE(stateObject, STATE_LOOP, JParse::Boolean, false);
        int posX = TRY_PARSE(stateObject, STATE_POS_X, JParse::Integer, 0);
        int posY = TRY_PARSE(stateObject, STATE_POS_Y, JParse::Integer, 0);

        // Apply data to node
        Node* stateNode = CreateNode();
        stateNode->SetNodeName(StringConvert::ToWStr(nodeName));
        stateNode->SetAnimationName(StringConvert::ToWStr(animName));
        stateNode->SetLoop(loop);
        stateNode->SetPosition({ posX, posY });
    }

    // Load transitions (Loop through states, use int iterator to index m_nodes array)
    JParse::Array* transitions = objectRoot->Get<JParse::Array>(TRANSITIONS_ROOT);
    for (unsigned int i = 0; i < m_nodes.size(); i++)
    {
        JParse::Object* stateObject = states->m_contents[i]->GetAs<JParse::Object>();
        if (!stateObject->Has(TRANSITIONS_ROOT))
        {
            continue;
        }

        // Load transition
        JParse::Array* transitionsForState = stateObject->Get<JParse::Array>(TRANSITIONS_ROOT);
        for (JParse::Item* transition : transitionsForState->m_contents)
        {
            // Get References
            JParse::Object* transitionObject = transition->GetAs<JParse::Object>();
            int toStateID = TRY_PARSE(transitionObject, STATE_TRANSITION_STATE, JParse::Integer, -1);
            int transitionID = TRY_PARSE(transitionObject, STATE_TRANSITION_TRANSITION_ID, JParse::Integer, -1);

            if (toStateID == -1 || transitionID == -1)
            {
                // Invalid transition
                continue;
            }

            Transition* transitionData = CreateTransition(i, toStateID);

            if (transitionData == nullptr)
            {
                continue;
            }

            // Load conditions
            JParse::Object* referencedTransition = transitions->m_contents[transitionID]->GetAs<JParse::Object>();
            std::string transitionName = TRY_PARSE(referencedTransition, TRANSITION_NAME, JParse::String, "New Transition");
            transitionData->SetName(StringConvert::ToWStr(transitionName));

            JParse::Array* conditions = referencedTransition->TryGet<JParse::Array>(TRANSITION_CONDITIONS);
            if (!conditions)
            {
                continue;
            }

            for (JParse::Item* condition : conditions->m_contents)
            {
                JParse::Object* conditionObject = condition->GetAs<JParse::Object>();
                std::string variableName = TRY_PARSE(conditionObject, CONDITION_VARIABLE_NAME, JParse::String, "NULL");
                std::string expectedType = TRY_PARSE(conditionObject, CONDITION_TYPE, JParse::String, "NULL");
                std::string expectedOperator = TRY_PARSE(conditionObject, CONDITION_OPERATION, JParse::String, "NULL");

                TransitionCondition conditionData;

                conditionData.SetVariableName(StringConvert::ToWStr(variableName));
                if (strcmp(expectedType.c_str(), "Boolean") == 0)
                {
                    conditionData.SetConditionType(OperatorType::EQUAL);
                    conditionData.SetVariableType(VariableType::TYPE_BOOL);
                    conditionData.GetVariable().m_bool = true;
                }
                else
                {
                    conditionData.SetVariableTypeFromString(expectedType);
                    conditionData.SetOperatorFromString(expectedOperator);

                    Variable v;
                    switch (conditionData.GetVariableType())
                    {
                    default:
                    case VariableType::TYPE_INT:
                        v.m_int = TRY_PARSE(conditionObject, CONDITION_VALUE, JParse::Integer, 0);
                        break;
                    case VariableType::TYPE_FLOAT:
                        v.m_float = TRY_PARSE(conditionObject, CONDITION_VALUE, JParse::Float, 0.0f);
                        break;
                    case VariableType::TYPE_BOOL:
                        v.m_bool = TRY_PARSE(conditionObject, CONDITION_VALUE, JParse::Boolean, false);
                        break;
                    }
                    conditionData.SetVariable(v);
                }

                transitionData->GetConditions().push_back(conditionData);
            }
        }
    }

    // Invalidate screen to ensure a full refresh
    RECT clientRect;
    m_containsUnsavedChanges = false;
    GetClientRect(m_mainWindowHandle, &clientRect);
    InvalidateRect(m_mainWindowHandle, &clientRect, true);
    return true;
}

// Ideally this would be better
// But I just want to get this project done tbh
bool GraphmationForgeApp::SaveJSON(std::string const& path)
{
    // Build JSON
    JParse::Root json;
    JParse::Object* rootObject = new JParse::Object;
    json.m_item = rootObject;

    JParse::Array* statesArray = new JParse::Array;
    rootObject->m_contents[STATES_ROOT] = statesArray;

    // Allow us to use duplicate transitions
    std::vector<Transition*> transitionsToSerialize;

    // Build state array
    for (Node const* const state : m_nodes)
    {
        JParse::Object* stateObject = new JParse::Object;
        statesArray->Add(stateObject);

        SET_DATA(stateObject, STATE_NAME, JParse::String, StringConvert::ToStr(state->GetNodeName()));
        SET_DATA(stateObject, STATE_ANIM_NAME, JParse::String, StringConvert::ToStr(state->GetAnimationName()));
        SET_DATA(stateObject, STATE_LOOP, JParse::Boolean, state->GetLoop());
        SET_DATA(stateObject, STATE_POS_X, JParse::Integer, state->GetNodePosition().x);
        SET_DATA(stateObject, STATE_POS_Y, JParse::Integer, state->GetNodePosition().y);

        // Build transition reference array
        JParse::Array* transitionsFromState = new JParse::Array;

        std::vector<Transition*> transitionDataFromState = GetTransitionsAttachedFromNode(state);
        for (Transition* transitionData : transitionDataFromState)
        {
            JParse::Object* transitionObject = new JParse::Object;
            transitionsFromState->Add(transitionObject);

            int const stateID = GetStateID(transitionData->GetToNode());
            SET_DATA(transitionObject, STATE_TRANSITION_STATE, JParse::Integer, stateID);

            // Check if we have already tracked a transition with the same conditions, if so then just register a reference to that
            // Otherwise, add this to the list and track it
            bool foundTransitionToReuse = false;
            for (unsigned int i = 0; i < transitionsToSerialize.size(); i++)
            {
                if (transitionData->HasSameConditions(transitionsToSerialize[i]))
                {
                    SET_DATA(transitionObject, STATE_TRANSITION_TRANSITION_ID, JParse::Integer, i);
                    foundTransitionToReuse = true;
                    break;
                }
            }

            if (!foundTransitionToReuse)
            {
                SET_DATA(transitionObject, STATE_TRANSITION_TRANSITION_ID, JParse::Integer, transitionsToSerialize.size());
                transitionsToSerialize.push_back(transitionData);
            }
        }

        stateObject->Set(TRANSITIONS_ROOT, transitionsFromState);
    }

    JParse::Array* transitionsArray = new JParse::Array;
    rootObject->m_contents[TRANSITIONS_ROOT] = transitionsArray;
    // Build transitions array
    for (Transition const* const transition : transitionsToSerialize)
    {
        JParse::Object* transitionObject = new JParse::Object;
        transitionsArray->Add(transitionObject);

        SET_DATA(transitionObject, TRANSITION_NAME, JParse::String, StringConvert::ToStr(transition->GetName()));

        // Build conditions array
        JParse::Array* conditionsArray = new JParse::Array;
        transitionObject->Set(TRANSITION_CONDITIONS, conditionsArray);
        for (TransitionCondition const& condition : transition->GetConditions())
        {
            JParse::Object* conditionObject = new JParse::Object;
            condition.BuildJSON(conditionObject);
            conditionsArray->Add(conditionObject);
        }

        transitionObject->Set(TRANSITION_CONDITIONS, conditionsArray);
    }

    json.SaveToFile(path);

    m_containsUnsavedChanges = false;

    int msgboxID = MessageBox(
        NULL,
        L"Saved successfully!",
        L"Save Success",
        MB_ICONINFORMATION | MB_OK
    );

    return true;
}

bool GraphmationForgeApp::TryUnloadGraph()
{
    if (m_containsUnsavedChanges)
    {
        int msgboxID = MessageBox(
            NULL,
            L"Warning, unsaved changes will be discarded! Save changes before closing graph?",
            L"Discarding Changes",
            MB_ICONWARNING | MB_YESNOCANCEL
        );

        if (msgboxID == IDYES)
        {
            SaveFile();
        }
        else if (msgboxID == IDCANCEL)
        {
            return false;
        }
    }

    // Unload graph
    for (Node* node : m_nodes)
    {
        delete node;
    }
    m_nodes.clear();

    for (Transition* transition : m_transitions)
    {
        delete transition;
    }
    m_transitions.clear();

    m_potentialSelectable = nullptr;
    m_selectedObjects.clear();

    m_loadedFilepath = "";
    m_containsUnsavedChanges = false;

    m_isDragging = false;

    m_currentAppState = Default;
    m_createTransitionStartingNode = nullptr;
    return true;
}

bool GraphmationForgeApp::InitInstance(int cmdShow)
{
    m_mainWindowHandle = CreateWindowW(m_stringResources[IDC_GRAPHMATIONFORGE], m_stringResources[IDS_APP_TITLE], WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, nullptr, m_instanceHandle, nullptr);
           
    if (!m_mainWindowHandle)
    {
        return FALSE;
    }

    ShowWindow(m_mainWindowHandle, cmdShow);
    UpdateWindow(m_mainWindowHandle);

    return TRUE;
}

void GraphmationForgeApp::LoadStringResources()
{
    LoadStringResource(IDS_APP_TITLE);
    LoadStringResource(IDC_GRAPHMATIONFORGE);
    LoadStringResource(ID_CLASS_NODE);
    LoadStringResource(ID_CLASS_PROPERTIES_PANEL);
}

void GraphmationForgeApp::CreateBrushPalette()
{
    m_brushes.resize(ID_COLOR_COUNT);
    for (int i = 0; i < ID_COLOR_COUNT; i++)
    {
        CreateBrush(i, g_colors[i]);
    }
}

void GraphmationForgeApp::CreateFonts()
{
    CreateWindowFont(L"Segoe UI", 25, FW_BOLD); // 0
}

void GraphmationForgeApp::RegisterWindowClasses()
{
    RegisterWindowClass(m_stringResources[IDC_GRAPHMATIONFORGE], m_brushes[ID_COLOR_BG]); // Main window
    RegisterWindowClass(m_stringResources[ID_CLASS_NODE], m_brushes[ID_COLOR_NODE_DESELECTED]);
    RegisterWindowClass(m_stringResources[ID_CLASS_PROPERTIES_PANEL], m_brushes[ID_COLOR_NODE_HIGHLIGHTED]);
}

Node* const GraphmationForgeApp::CreateNode()
{
    HWND nodeWindowHandle = 
        CreateWindow(m_stringResources[ID_CLASS_NODE],
                     L"Node",
                     WS_CHILD | WS_VISIBLE,
                     10, 10, NODE_MIN_WIDTH, NODE_HEIGHT,
                     m_mainWindowHandle,
                     (HMENU)ID_CLASS_NODE, NULL, NULL);
    HRGN region = CreateRoundRectRgn(0, 0, NODE_MIN_WIDTH, NODE_HEIGHT, 20, 20);
    SetWindowRgn(nodeWindowHandle, region, true);

    Node* node = new Node(m_mainWindowHandle, nodeWindowHandle);
    m_nodes.push_back(node);

    POINT p;
    p.x = 10;
    p.y = 10;
    node->SetPosition(p);

    m_containsUnsavedChanges = true;

    return node;
}

Node* const GraphmationForgeApp::CreateNodeAtMousePos()
{
    Node* node = CreateNode();
    POINT mousePos;
    GetCursorPos(&mousePos);
    ScreenToClient(m_mainWindowHandle, &mousePos);
    node->SetPosition(mousePos);
    return node;
}

Transition * const GraphmationForgeApp::CreateTransition(unsigned int const fromNodeID, unsigned int const toNodeID)
{
    if (fromNodeID >= m_nodes.size() || toNodeID >= m_nodes.size())
    {
        return nullptr;
    }

    return CreateTransition(m_nodes[fromNodeID], m_nodes[toNodeID]);
}

Transition * const GraphmationForgeApp::CreateTransition(Node * const fromNode, Node * const toNode)
{
    Transition* transition = new Transition(m_mainWindowHandle);
    transition->SetFromNode(fromNode);
    transition->SetToNode(toNode);

    std::vector<Transition*> siblingTransitions = GetTransitionsAttachedFromNode(toNode);
    for (Transition* const sibling : siblingTransitions)
    {
        sibling->InvalidatePaintArea();
    }
    
    m_transitions.push_back(transition);
    transition->InvalidatePaintArea();

    for (Transition* const sibling : siblingTransitions)
    {
        sibling->InvalidatePaintArea();
    }

    m_containsUnsavedChanges = true;
    return transition;
}

void GraphmationForgeApp::InvalidateAttachedTransitions(std::unordered_set<ISelectable*> const & selectedObjects)
{
    std::unordered_map<Transition*, bool> attachedTransitions;
    for (ISelectable* selectable : selectedObjects)
    {
        if (Node* node = dynamic_cast<Node*>(selectable))
        {
            for (Transition* t : m_transitions)
            {
                if (t->GetFromNode() == node || t->GetToNode() == node)
                {
                    attachedTransitions[t] = true;
                }
            }
        }
    }

    for (auto const& transition : attachedTransitions)
    {
        if (transition.second)
        {
            transition.first->InvalidatePaintArea();
        }
    }

}

bool const GraphmationForgeApp::AreNodesTwoWayConnected(Node const* const nodeA, Node const* const nodeB)
{
    bool aToB = false;
    bool bToA = false;
    for (Transition* transition : m_transitions)
    {
        if (transition->GetFromNode() == nodeA && transition->GetToNode() == nodeB)
        {
            aToB = true;
        }
        else if (transition->GetFromNode() == nodeB && transition->GetToNode() == nodeA)
        {
            bToA = true;
        }
    }

    return aToB && bToA;
}

std::vector<Transition*> GraphmationForgeApp::GetTransitionsAttachedFromNode(Node const * const node)
{
    std::vector<Transition*> transitions;
    for (Transition* const transition : m_transitions)
    {
        if (transition->GetFromNode() == node)
        {
            transitions.push_back(transition);
        }
    }
    return transitions;
}

int const GraphmationForgeApp::GetStateID(Node const * const node) const
{
    for (unsigned int i = 0; i < m_nodes.size(); i++)
    {
        if (node == m_nodes[i])
        {
            return i;
        }
    }

    return -1;
}

void GraphmationForgeApp::LoadStringResource(int resourceID)
{
    LoadStringW(m_instanceHandle, resourceID, m_stringResources[resourceID], MAX_LOADSTRING);
}

void GraphmationForgeApp::CreateBrush(int brushID, COLORREF brushColor)
{
    m_brushes[brushID] = CreateSolidBrush(brushColor);
}

void GraphmationForgeApp::CreateWindowFont(const wchar_t* fontName, int const fontSize, int const fontWeight)
{
    m_fonts.push_back(CreateFont(fontSize, 0, 0, 0, fontWeight, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, fontName));
}
