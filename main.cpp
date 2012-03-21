#include <QtCore>

void printLine(const QString &line, int col)
{
    printf("%s\n", qPrintable(line));
    if (col) {
        for (int i=1; i<col; ++i) {
            printf(" ");
        }
        printf("^\n");
    }
}

int main(int argc, char **argv)
{
    QRegExp lineColRx("^(.*):([0-9]+):([0-9]+):?$");
    QRegExp lineRx("^(.*):([0-9]+):?$");
    QRegExp offsetRx("^(.*),([0-9]+)$");
    for (int i=1; i<argc; ++i) {
        int off = 0;
        int line = 0;
        int col = 0;
        QString fileName;
        if (lineColRx.exactMatch(argv[i])) {
            fileName = lineColRx.cap(1);
            line = lineColRx.cap(2).toInt();
            if (!line)
                return 1;
            col = lineColRx.cap(3).toInt();
            qDebug() << line << col << lineColRx.capturedTexts();
            if (!col)
                return 1;
        } else if (lineRx.exactMatch(argv[i])) {
            fileName = lineRx.cap(1);
            line = lineRx.cap(2).toInt();
            if (!line)
                return 1;
        } else if (offsetRx.exactMatch(argv[i])) {
            fileName = offsetRx.cap(1);
            off = offsetRx.cap(2).toInt();
            if (!off)
                return 1;
            ++off;
        } else {
            return 1;
        }
        QFile f(fileName);
        if (!f.open(QIODevice::ReadOnly))
            return 1;
        QTextStream ts(&f);
        QString l;
        if (line) {
            do {
                l = ts.readLine();
            } while (--line);
            printLine(l, col);
        } else {
            forever {
                l = ts.readLine();
                if (off > l.size() + 1) {
                    off -= (l.size() + 1);
                } else {
                    break;
                }
            }
            printLine(l, off);
        }
    }
    return 0;
}
