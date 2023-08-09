#include "mainwindow.h"
#include "code_editor.h"
#include "interpreter/interpreter.h"
#include <memory>
#include <qaction.h>
#include <qmenu.h>
using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
  MainWidget = make_unique<QWidget>();
  MainLayout = make_unique<QGridLayout>();
  TextBox = make_unique<CodeEditor>();
  FileName = "sin-nombre.mir";

  setCentralWidget(MainWidget.get());
  setWindowTitle(tr("Mimir"));
  MainWidget->setLayout(MainLayout.get());
  MainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

  CreateActions();
  CreateMenus();

  CompileBox = make_unique<QPlainTextEdit>();
  CompileBox->setReadOnly(true);

  MainLayout->addWidget(TextBox.get());
  MainLayout->addWidget(CompileBox.get());
  TextBox->setPlainText(tr(R"("Hola " + "Mundo!")"));
}

void MainWindow::CreateActions()
{
  // file menu actions

  ActNewFile = std::make_unique<QAction>(tr("&Nuevo"), this);
  ActNewFile->setStatusTip(tr("Crear"));
  connect(ActNewFile.get(), &QAction::triggered, this, &MainWindow::NewFile);

  ActLoadFile = std::make_unique<QAction>(tr("&Cargar"), this);
  ActLoadFile->setStatusTip(tr("Cargar"));
  connect(ActLoadFile.get(), &QAction::triggered, this, &MainWindow::LoadFile);

  ActSaveFile = std::make_unique<QAction>(tr("&Guardar"), this);
  ActSaveFile->setStatusTip(tr("Guardar"));
  connect(ActSaveFile.get(), &QAction::triggered, this, &MainWindow::SaveFile);

  ActSaveFileAs = std::make_unique<QAction>(tr("&Guardar como..."), this);
  ActSaveFile->setStatusTip(tr("Guardar como..."));
  connect(ActSaveFileAs.get(), &QAction::triggered, this,
          &MainWindow::SaveFileAs);

  ActExit = std::make_unique<QAction>(tr("&Salir"), this);
  ActExit->setStatusTip(tr("Salir del programa"));
  connect(ActExit.get(), &QAction::triggered, this, &MainWindow::close);

  // edit menu actions

  ActCut = std::make_unique<QAction>(tr("&Cortar"), this);
  ActCut->setStatusTip(tr("Cortar texto seleccionado"));
  connect(ActCut.get(), &QAction::triggered, this, &MainWindow::Cut);

  ActCopy = std::make_unique<QAction>(tr("&Copiar"), this);
  ActCopy->setStatusTip(tr("Copiar texto seleccionado"));
  connect(ActCopy.get(), &QAction::triggered, this, &MainWindow::Copy);

  ActPaste = std::make_unique<QAction>(tr("&Pegar"), this);
  ActPaste->setStatusTip(tr("Pegar texto"));
  connect(ActPaste.get(), &QAction::triggered, this, &MainWindow::Paste);

  ActUndo = std::make_unique<QAction>(tr("&Deshacer"), this);
  ActUndo->setStatusTip(tr("Deshacer"));
  connect(ActUndo.get(), &QAction::triggered, this, &MainWindow::Undo);

  ActRedo = std::make_unique<QAction>(tr("&Rehacer"), this);
  ActRedo->setStatusTip(tr("Rehacer"));
  connect(ActRedo.get(), &QAction::triggered, this, &MainWindow::Redo);

  ActSelectAll = std::make_unique<QAction>(tr("&Seleccionar todo"), this);
  ActSelectAll->setStatusTip(tr("Seleccionar todo el texto"));
  connect(ActSelectAll.get(), &QAction::triggered, this,
          &MainWindow::SelectAll);

  // interpreter menu actions
  ActExecuteCode = std::make_unique<QAction>(tr("Ejecutar"), this);
  ActExecuteCode->setStatusTip(tr("Ejecutar codigo"));
  connect(ActExecuteCode.get(), &QAction::triggered, this,
          &MainWindow::ExecuteCode);

  // about menu actions
  ActAbout = std::make_unique<QAction>(tr("Licencia"), this);
  connect(ActAbout.get(), &QAction::triggered, this, &MainWindow::About);
}

void MainWindow::CreateMenus()
{
  MenuFile = unique_ptr<QMenu>(menuBar()->addMenu(tr("&Archivo")));
  MenuFile->addAction(ActNewFile.get());
  MenuFile->addAction(ActLoadFile.get());
  MenuFile->addAction(ActSaveFile.get());
  MenuFile->addAction(ActSaveFileAs.get());
  MenuFile->addAction(ActExit.get());

  MenuEdit = unique_ptr<QMenu>(menuBar()->addMenu(tr("&Editar")));
  MenuEdit->addAction(ActCut.get());
  MenuEdit->addAction(ActCopy.get());
  MenuEdit->addAction(ActPaste.get());
  MenuEdit->addAction(ActUndo.get());
  MenuEdit->addAction(ActRedo.get());
  MenuEdit->addAction(ActSelectAll.get());

  MenuEdit = unique_ptr<QMenu>(menuBar()->addMenu(tr("&Interprete")));
  MenuEdit->addAction(ActExecuteCode.get());

  MenuAbout = unique_ptr<QMenu>(menuBar()->addMenu(tr("&Sobre")));
  MenuAbout->addAction(ActAbout.get());
}

void MainWindow::NewFile()
{
  FileName = "sin-titulo";
  TextBox->setPlainText("");
}

void MainWindow::LoadFile()
{
  FileName = QFileDialog::getOpenFileName(this, "Abrir archivo");
  QFile file(FileName);
  if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
    QMessageBox::warning(this, "Advertencia!",
                         "No se puede abrir el archivo: " + file.errorString());
    return;
  }
  setWindowTitle(FileName);
  QTextStream in(&file);
  TextBox->setPlainText(in.readAll());
  file.close();
}

void MainWindow::SaveFile()
{
  QFile file(FileName);
  if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
    QMessageBox::warning(this, "Advertencia!",
                         "No se puede abrir el archivo: " + file.errorString());
    return;
  }
  setWindowTitle(FileName);
  QTextStream out(&file);
  out << TextBox->toPlainText();
  file.close();
}

void MainWindow::SaveFileAs()
{
  FileName = QFileDialog::getSaveFileName(this, "Guardar como...");
  QFile file(FileName);
  if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
    QMessageBox::warning(this, "Advertencia!",
                         "No se puede abrir el archivo: " + file.errorString());
    return;
  }
  setWindowTitle(FileName);
  QTextStream out(&file);
  out << TextBox->toPlainText();
  file.close();
}

void MainWindow::Copy() { TextBox->copy(); }

void MainWindow::Cut() { TextBox->cut(); }

void MainWindow::Paste() { TextBox->paste(); }

void MainWindow::Undo() { TextBox->undo(); }

void MainWindow::Redo() { TextBox->redo(); }

void MainWindow::SelectAll() { TextBox->selectAll(); }

void MainWindow::ExecuteCode()
{
  console.append(">> " +
                 QString::fromStdString(
                     interprete_code(TextBox->toPlainText().toStdString())) +
                 "\n");
  CompileBox->setPlainText(console);
}

void MainWindow::About()
{
  QMessageBox::about(
      this, tr("Sobre este programa"),
      tr("Este es un software educativo para estudiantes de programación\n\n"

         "Copyright © 2023 Odanis Esquea\n\n"

         "Permission is hereby granted, free of charge, to any person "
         "obtaining a copy "
         "of this software and associated documentation files (the "
         "\"Software\"), to deal "
         "in the Software without restriction, including without limitation "
         "the rights "
         "to use, copy, modify, merge, publish, distribute, sublicense, and/or "
         "sell "
         "copies of the Software, and to permit persons to whom the Software "
         "is "
         "furnished to do so, subject to the following conditions: \n\n"

         "The above copyright notice and this permission notice shall be "
         "included in all "
         "copies or substantial portions of the Software.\n\n"

         "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, "
         "EXPRESS OR "
         "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF "
         "MERCHANTABILITY, "
         "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT "
         "SHALL THE "
         "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR "
         "OTHER "
         "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, "
         "ARISING FROM, "
         "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER "
         "DEALINGS IN THE "
         "SOFTWARE."));
}
