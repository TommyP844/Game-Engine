#include "EditorState.h"

static EditorState* sEditorState;

void GenerateEditorState()
{
	sEditorState = new EditorState();
	sEditorState->ProjectPath = "C:\\Mule\\Projects\\Sample Project";
}

void DestoyEditorState()
{
	delete sEditorState;
}

EditorState& GetEditorState()
{
	return *sEditorState;
}
