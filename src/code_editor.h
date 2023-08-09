#ifndef CODE_EDITOR_H
#define CODE_EDITOR_H

#include <QPlainTextEdit>
#include <QWidget>
#include <memory>

class CodeEditor : public QPlainTextEdit {

public:
  explicit CodeEditor(QWidget *parent = nullptr);
  ~CodeEditor() override = default;
  void lineNumberAreaPaintEvent(QPaintEvent *event);
  auto lineNumberAreaWidth() -> int;

protected:
  void resizeEvent(QResizeEvent *event) override;

private slots:
  void updateLineNumberAreaWidth(int newBlockCount);
  void highlightCurrentLine();
  void updateLineNumberArea(const QRect &, int);

private:
  std::unique_ptr<QWidget> lineNumberArea;
};

class LineNumberArea : public QWidget {
public:
  explicit LineNumberArea(CodeEditor *editor)
      : QWidget(editor), codeEditor(editor) {}

  [[nodiscard]] auto sizeHint() const -> QSize override
  {
    return {codeEditor->lineNumberAreaWidth(), 0};
  }

protected:
  void paintEvent(QPaintEvent *event) override
  {
    codeEditor->lineNumberAreaPaintEvent(event);
  }

private:
  CodeEditor *codeEditor;
};

#endif // !CODE_EDITOR_H
