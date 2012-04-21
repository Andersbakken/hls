#include <QtCore>

void printLine(const QString &line, int col, bool context)
{
    if (col < 0) {
        printf("%s\n", qPrintable(line));
    } else {
        int idx = line.indexOf(QRegExp("[^A-Za-z0-9_]"), col);
        if (idx == -1)
            idx = line.size();
        static const char *color = "\x1b[32;1m"; // dark yellow
        static const char *resetColor = "\x1b[0;0m";

        printf("%s%s%s%s%s", qPrintable(line.left(col)),
               color, qPrintable(line.mid(col, idx - col)),
               resetColor, qPrintable(line.mid(idx)));
        if (context) {
            printf("    %s<---%s\n", color, resetColor);
        } else {
            printf("\n");
        }
    }
}

void printLine(const QString &string)
{
    if (!string.isEmpty())
        printf("%s\n", qPrintable(string));
}

int main(int argc, char **argv)
{
    QRegExp lineColRx("^(.*):([0-9]+):([0-9]+):?$");
    QRegExp lineRx("^(.*):([0-9]+):?$");
    QRegExp offsetRx("^(.*),([0-9]+)$");
    bool context = false;
    for (int i=1; i<argc; ++i) {
        if (!strcmp(argv[i], "--context") || !strcmp(argv[i], "-c")) {
            context = true;
            continue;
        }
        int off = 0;
        int line = 0;
        int col = 0;
        QString fileName;
        if (lineColRx.exactMatch(argv[i])) {
            fileName = lineColRx.cap(1);
            line = lineColRx.cap(2).toInt();
            if (!line)
                return 1;
            bool ok;
            col = lineColRx.cap(3).toInt(&ok);
            if (!ok || col < 0)
                return 1;
        } else if (lineRx.exactMatch(argv[i])) {
            fileName = lineRx.cap(1);
            line = lineRx.cap(2).toInt();
            if (!line)
                return 1;
        } else if (offsetRx.exactMatch(argv[i])) {
            fileName = offsetRx.cap(1);
            bool ok;
            off = offsetRx.cap(2).toInt(&ok);
            if (!ok)
                return 1;
        } else {
            return 1;
        }
        QFile f(fileName);
        if (!f.open(QIODevice::ReadOnly))
            return 1;
        QTextStream ts(&f);
        QString l;
        QString last, secondLast;
        if (line) {
            while (!ts.atEnd()) {
                secondLast = last;
                last = l;
                l = ts.readLine();
                if (!--line)
                    break;
            }
            if (line)
                return 1;
            if (context) {
                printLine(secondLast);
                printLine(last);
            }
            printLine(l, col, context);
            if (context) {
                for (int i=0; i<2 && !ts.atEnd(); ++i) {
                    printLine(ts.readLine());
                }
            }
        } else {
            forever {
                secondLast = last;
                last = l;
                l = ts.readLine();
                if (off > l.size() + 1) {
                    off -= (l.size() + 1);
                } else {
                    break;
                }
                if (ts.atEnd())
                    return 1;
            }
            if (context) {
                printLine(secondLast);
                printLine(last);
            }
            printLine(l, off, context);
            if (context) {
                for (int i=0; i<2 && !ts.atEnd(); ++i) {
                    printLine(ts.readLine());
                }
            }
        }
    }
    return 0;
}
