#include "GraphmationForgeApp.h"

#include "GraphmationColors.h"
#include "Node.h"
#include "Transition.h"
#include "StringConvert.h"

#include "JParse/JParse.h"
#include "ParsingDefines.h"

#include <windowsx.h>

GraphmationForgeApp* GraphmationForgeApp::s_instance;

// Global callback for Win32
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    GraphmationForgeApp* instance = GraphmationForgeApp::GetInstance();
    int retValue = -1;
    switch (message)
    {
    case WM_CREATE:
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
    case WM_DESTROY:
        // TODO: CHECK FOR UNSAVED DOCUMENT
        PostQuitMessage(0);
        return 0;
    }

    if (retValue == -1)
    {
        // Window refused to process command, fallback to default behavior
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

GraphmationForgeApp::GraphmationForgeApp()
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
    default:
        return -1;
    }

    return 0;
}

int GraphmationForgeApp::OnWindowCommand(WIN32_CALLBACK_PARAMS)
{
    int commandID = LOWORD(wParam);
    // Parse the menu selections:
    switch (commandID)
    {
    case IDM_EXIT:
        DestroyWindow(hWnd);
        break;
    case IDM_OPEN:
        OpenFile();
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

        break;
    default:
        return -1;
    }
    return 0;
}

int GraphmationForgeApp::OnLeftMouseButtonDown(WIN32_CALLBACK_PARAMS)
{   
    // if (hWnd != m_mainWindowHandle)
    // {
    //     // Only interpret mouse button down for the main window
    //     return -1;
    // }

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
            // TODO: Don't assume node
            Node* node = static_cast<Node*>(selectable);
            node->StartDrag(mousePos);
        }

        m_isDragging = true;
    }
    else
    {
        bool found = false;
        for(int i = 0; i < m_selectedObjects.size(); i++)
        {
            if (m_selectedObjects[i] == m_potentialSelectable)
            {
                // Deselect if holding shift or already selcted, otherwise deselect all others
                if (wParam & MK_SHIFT || m_potentialSelectable->GetSelectionState() == SELECTED)
                {
                    m_potentialSelectable->SetSelectionState(NONE);
                    m_selectedObjects.erase(m_selectedObjects.begin() + i);
                    found = true;
                }
                else
                {
                    DeselectAll();
                }


                break;
            }
        }

        if (!found)
        {
            int shift = wParam & MK_SHIFT;
            if (!shift)
            {
                DeselectAll();
            }

            m_potentialSelectable->SetSelectionState(SELECTED);
            m_selectedObjects.push_back(m_potentialSelectable);
        }
    }

    return 0;
}

int GraphmationForgeApp::OnLeftMouseButtonUp(WIN32_CALLBACK_PARAMS)
{
    // if (hWnd != m_mainWindowHandle)
    // {
    //     // Only interpret mouse button up for the main window
    //     return -1;
    // }

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

    return 0;
}

int GraphmationForgeApp::OnOpenContextMenu(WIN32_CALLBACK_PARAMS)
{
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

void GraphmationForgeApp::OnMainWindowCreated(WIN32_CALLBACK_PARAMS)
{
    // Create all default sub-windows
}

void GraphmationForgeApp::PaintMainWindow(WIN32_CALLBACK_PARAMS)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    for (Transition* transition : m_transitions)
    {
        transition->Paint(hdc, m_brushes[ID_COLOR_TRANSITION_DESELECTED]);
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

void GraphmationForgeApp::DeselectAll()
{
    for (ISelectable* selectable : m_selectedObjects)
    {
        selectable->SetSelectionState(NONE);
    }
    m_selectedObjects.clear();
}

bool GraphmationForgeApp::OpenFile()
{
    if (!TryUnloadGraph())
    {
        return false;
    }

    LPCTSTR filter = L"Animation Graph (*.animgraph)|*.animgraph|";

    CFileDialog dlgFile(TRUE, _T("animgraph"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, filter, CWnd::FromHandle(m_mainWindowHandle));

    if (dlgFile.DoModal() != IDOK)
    {
        return false;
    }

    CString pathCStr(dlgFile.GetPathName().GetString());
    std::wstring path(pathCStr);

    return LoadJSON(StringConvert::ToStr(path));
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

    return SaveJSON(StringConvert::ToStr(path));
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
                conditionData.m_variableName = StringConvert::ToWStr(variableName);
                conditionData.SetVariableTypeFromString(expectedType);
                conditionData.SetOperatorFromString(expectedOperator);

                Variable v;
                switch (conditionData.m_expectedType)
                {
                default:
                case TYPE_INT:
                    v.m_int = TRY_PARSE(conditionObject, CONDITION_VALUE, JParse::Integer, 0);
                    break;
                case TYPE_FLOAT:
                    v.m_float = TRY_PARSE(conditionObject, CONDITION_VALUE, JParse::Float, 0.0f);
                    break;
                case TYPE_BOOL:
                    v.m_bool = TRY_PARSE(conditionObject, CONDITION_VALUE, JParse::Boolean, false);
                    break;
                }
                conditionData.m_value = v;

                transitionData->GetConditions().push_back(conditionData);
            }
        }
    }

    // Invalidate screen to ensure a full refresh
    RECT clientRect;
    GetClientRect(m_mainWindowHandle, &clientRect);
    InvalidateRect(m_mainWindowHandle, &clientRect, true);
    return true;
}

bool GraphmationForgeApp::SaveJSON(std::string const& path)
{
    // Build JSON
    JParse::Root json;
    JParse::Object* rootObject = new JParse::Object;
    json.m_item = rootObject;

    JParse::Array* transitionsArray = new JParse::Array;
    JParse::Array* statesArray = new JParse::Array;
    rootObject->m_contents[TRANSITIONS_ROOT] = transitionsArray;
    rootObject->m_contents[STATES_ROOT] = statesArray;

    // Build state array
    for (Node* state : m_nodes)
    {
        JParse::Object* stateObject = new JParse::Object;
        statesArray->Add(stateObject);

        SET_DATA(stateObject, STATE_NAME, JParse::String, StringConvert::ToStr(state->GetNodeName()));
        SET_DATA(stateObject, STATE_ANIM_NAME, JParse::String, StringConvert::ToStr(state->GetAnimationName()));
        SET_DATA(stateObject, STATE_LOOP, JParse::Boolean, state->GetLoop());
        SET_DATA(stateObject, STATE_POS_X, JParse::Integer, state->GetNodePosition().x);
        SET_DATA(stateObject, STATE_POS_Y, JParse::Integer, state->GetNodePosition().y);
    }

    json.SaveToFile(path);
    return true;
}

bool GraphmationForgeApp::TryUnloadGraph()
{
    return !m_containsUnsavedChanges;
    return false;
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
}

Node* const GraphmationForgeApp::CreateNode()
{
    HWND nodeWindowHandle = 
        CreateWindow(m_stringResources[ID_CLASS_NODE],
                     L"Node",
                     WS_CHILD | WS_VISIBLE,
                     10, 10, NODE_WIDTH, NODE_HEIGHT,
                     m_mainWindowHandle,
                     (HMENU)ID_CLASS_NODE, NULL, NULL);
    HRGN region = CreateRoundRectRgn(0, 0, NODE_WIDTH, NODE_HEIGHT, 20, 20);
    SetWindowRgn(nodeWindowHandle, region, true);

    Node* node = new Node(m_mainWindowHandle, nodeWindowHandle);
    m_nodes.push_back(node);

    POINT p;
    p.x = 10;
    p.y = 10;
    node->SetPosition(p);

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
    transition->InvalidatePaintArea();

    m_transitions.push_back(transition);
    return transition;
}

void GraphmationForgeApp::InvalidateAttachedTransitions(std::vector<ISelectable*> const & selectedObjects)
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
