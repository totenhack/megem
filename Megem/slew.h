#pragma once

#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <regex>

#include "array.h"
#include "syntax.h"
#include "tree.h"
#include "eval.h"

PromptProc(SlewProc);
void SlewMain(char *path);
void MirrorsEdgeLibrary();

using namespace std;