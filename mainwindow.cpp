#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QFrame>
#include <QPixmap>
#include <QPainter>
#include <QMessageBox>
#include <QDialog>
#include <QLineEdit>
#include <QFormLayout>
#include <QEvent>
#include <QUrl>
#include <QApplication>

QPixmap createHackTimeLogoPixmap() {
    QPixmap pixmap(220, 60);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF iconRect(5, 5, 50, 50);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(13, 27, 42));
    painter.drawRoundedRect(iconRect, 10, 10);

    QRectF clockRect(10, 10, 40, 40);

    QPen bluePen(QColor(0, 122, 255), 3);
    painter.setPen(bluePen);
    painter.drawArc(clockRect, 45 * 16, 270 * 16);

    QPen orangePen(QColor(242, 107, 15), 3);
    painter.setPen(orangePen);
    painter.drawArc(clockRect, 15 * 16, 60 * 16);

    QFont codeFont("Arial", 8, QFont::Bold);
    painter.setFont(codeFont);
    painter.setPen(QColor(242, 107, 15));
    painter.drawText(QRectF(20, 6, 20, 12), Qt::AlignCenter, "</>");

    painter.setPen(QPen(QColor(0, 122, 255), 2, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(30, 30, 22, 22);
    painter.drawLine(30, 30, 42, 24);

    painter.setPen(QPen(QColor(242, 107, 15), 2));
    painter.drawLine(14, 30, 17, 30);
    painter.drawLine(43, 30, 46, 30);
    painter.drawLine(30, 43, 30, 46);

    QFont titleFont("Arial", 18, QFont::Bold);
    painter.setFont(titleFont);

    painter.setPen(QColor(255, 255, 255));
    painter.drawText(65, 32, "Hack");

    painter.setPen(QColor(242, 107, 15));
    painter.drawText(122, 32, "Time");

    QFont subFont("Arial", 5, QFont::Bold);
    painter.setFont(subFont);
    painter.setPen(QColor(148, 163, 184));
    painter.drawText(66, 45, "GESTION INTELLIGENTE DE HACKATHONS");

    return pixmap;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupSoundEffects();
    setupCustomUi();
    applyStyleSheet();
    setupLogoAnimation();

    // Event filter global 3al application kolha bech ay bouton tet'nzel ya3mel sout
    if (qApp) {
        qApp->installEventFilter(this);
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupSoundEffects()
{
    clickSound = new QSoundEffect(this);
    // Assurez-vous d'avoir un fichier click.wav ou modifiez le chemin si nécessaire
    clickSound->setSource(QUrl::fromLocalFile("click.wav"));
    clickSound->setVolume(0.7f);
}

void MainWindow::playClickSound()
{
    if (clickSound) {
        if (clickSound->isPlaying()) {
            clickSound->stop();
        }
        clickSound->play();
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QPushButton *btn = qobject_cast<QPushButton*>(watched);
        if (btn) {
            playClickSound();
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::setupLogoAnimation()
{
    if (!logoLabel) return;

    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(logoLabel);
    logoLabel->setGraphicsEffect(effect);

    QPropertyAnimation *anim = new QPropertyAnimation(effect, "opacity", this);
    anim->setDuration(1500);
    anim->setStartValue(0.4);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::InOutQuad);
    anim->setLoopCount(-1);
    anim->start();
}

QWidget* MainWindow::createActionButtonsWidget(int row)
{
    QWidget *actionsWidget = new QWidget();
    QHBoxLayout *actionsLayout = new QHBoxLayout(actionsWidget);
    actionsLayout->setContentsMargins(0, 0, 0, 0);
    actionsLayout->setSpacing(6);
    actionsLayout->setAlignment(Qt::AlignCenter);

    QPushButton *btnVoir = new QPushButton("👁");
    QPushButton *btnModif = new QPushButton("✏");
    QPushButton *btnSuppr = new QPushButton("🗑️");

    QString btnStyle = "QPushButton { border: none; background: transparent; font-size: 14px; cursor: pointer; }"
                       "QPushButton:hover { font-size: 16px; }";
    btnVoir->setStyleSheet(btnStyle);
    btnModif->setStyleSheet(btnStyle);
    btnSuppr->setStyleSheet(btnStyle);

    connect(btnVoir, &QPushButton::clicked, this, [this, row]() { onVoirParticipantClicked(row); });
    connect(btnModif, &QPushButton::clicked, this, [this, row]() { onModifierParticipantClicked(row); });
    connect(btnSuppr, &QPushButton::clicked, this, [this, row]() { onSupprimerParticipantClicked(row); });

    actionsLayout->addWidget(btnVoir);
    actionsLayout->addWidget(btnModif);
    actionsLayout->addWidget(btnSuppr);

    return actionsWidget;
}

void MainWindow::updateCounters()
{
    if (lblActifs && table) {
        lblActifs->setText(QString::number(table->rowCount()));
    }
    if (lblNouveaux) {
        lblNouveaux->setText(QString::number(nouveauxInscritsCount));
    }
}

void MainWindow::setupCustomUi()
{
    this->resize(1300, 850);
    this->setWindowTitle("Hackathon - HackTime");

    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 1. SIDEBAR
    QFrame *sidebar = new QFrame();
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(250);

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(15, 20, 15, 20);
    sidebarLayout->setSpacing(10);

    logoLabel = new QLabel();
    logoLabel->setPixmap(createHackTimeLogoPixmap());
    sidebarLayout->addWidget(logoLabel);
    sidebarLayout->addSpacing(25);

    struct MenuItem {
        QString name;
        QString icon;
    };

    const QList<MenuItem> menuItems = {
        {"Tableau de bord", "📊"},
        {"Compétitions", "🏆"},
        {"Équipes", "👥"},
        {"Participants", "👤"},
        {"Jury", "⚖️"},
        {"Utilisateurs", "⚙"}
    };

    for (const MenuItem &item : menuItems) {
        QPushButton *btn = new QPushButton(QString("  %1   %2").arg(item.icon, item.name));
        btn->setCursor(Qt::PointingHandCursor);
        if (item.name == "Participants") {
            btn->setObjectName("activeBtn");
        } else {
            btn->setObjectName("menuBtn");
        }
        sidebarLayout->addWidget(btn);
    }

    sidebarLayout->addStretch();

    QLabel *sidebarFooter = new QLabel("Des idées aujourd'hui,\nles solutions de demain");
    sidebarFooter->setStyleSheet("color: #64748b; font-style: italic; font-size: 11px; padding-left: 5px;");
    sidebarLayout->addWidget(sidebarFooter);

    mainLayout->addWidget(sidebar);

    // 2. MAIN CONTENT AREA
    QWidget *contentArea = new QWidget();
    contentArea->setObjectName("contentArea");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(25, 20, 25, 20);
    contentLayout->setSpacing(15);

    QLabel *pageTitle = new QLabel("Gestion des Participants");
    pageTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: #0f172a;");
    contentLayout->addWidget(pageTitle);

    // STATS CARDS
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(20);

    QFrame *card1 = new QFrame();
    card1->setObjectName("card");
    QVBoxLayout *c1Layout = new QVBoxLayout(card1);
    QLabel *c1Title = new QLabel("Participants Actifs");
    c1Title->setStyleSheet("color: #475569; font-weight: 600; font-size: 13px;");
    lblActifs = new QLabel("5");
    lblActifs->setStyleSheet("font-size: 28px; font-weight: bold; color: #0f172a;");
    c1Layout->addWidget(c1Title);
    c1Layout->addWidget(lblActifs);
    statsLayout->addWidget(card1);

    QFrame *card2 = new QFrame();
    card2->setObjectName("card");
    QVBoxLayout *c2Layout = new QVBoxLayout(card2);
    QLabel *c2Title = new QLabel("Nouveaux Inscrits");
    c2Title->setStyleSheet("color: #475569; font-weight: 600; font-size: 13px;");
    lblNouveaux = new QLabel("0");
    lblNouveaux->setStyleSheet("font-size: 28px; font-weight: bold; color: #ea580c;");
    c2Layout->addWidget(c2Title);
    c2Layout->addWidget(lblNouveaux);
    statsLayout->addWidget(card2);

    contentLayout->addLayout(statsLayout);

    // MIDDLE SECTION
    QHBoxLayout *middleLayout = new QHBoxLayout();
    middleLayout->setSpacing(20);

    QFrame *tableCard = new QFrame();
    tableCard->setObjectName("card");
    QVBoxLayout *tcLayout = new QVBoxLayout(tableCard);

    QHBoxLayout *tableHeader = new QHBoxLayout();
    QLabel *tableTitle = new QLabel("Liste des Participants");
    tableTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #0f172a;");

    QPushButton *addBtn = new QPushButton("+ Ajouter un Participant");
    addBtn->setObjectName("primaryBtn");
    addBtn->setCursor(Qt::PointingHandCursor);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAjouterParticipantClicked);

    tableHeader->addWidget(tableTitle);
    tableHeader->addStretch();
    tableHeader->addWidget(addBtn);
    tcLayout->addLayout(tableHeader);

    // TABLE
    table = new QTableWidget(5, 8);
    table->setHorizontalHeaderLabels({"ID", "Nom Comple", "E-mail", "Université", "Compétence", "Équipe", "Statut", "Actions"});

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    table->setColumnWidth(6, 120);
    table->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);

    table->verticalHeader()->setVisible(false);
    table->verticalHeader()->setDefaultSectionSize(45);
    table->setShowGrid(false);

    for(int i = 0; i < 5; ++i) {
        table->setItem(i, 0, new QTableWidgetItem(QString("PA-00%1").arg(i+1)));
        table->setItem(i, 1, new QTableWidgetItem(i == 2 ? "Sorah Benali" : "Sarah Benali"));
        table->setItem(i, 2, new QTableWidgetItem(i == 2 ? "sararh.b@uni..." : "sarah.b@uni..."));
        table->setItem(i, 3, new QTableWidgetItem("INSAT"));
        table->setItem(i, 4, new QTableWidgetItem("Dev Full-Stack"));
        table->setItem(i, 5, new QTableWidgetItem("Team ..."));

        QLabel *status = new QLabel(i < 2 ? "Done" : "In Progress");
        status->setAlignment(Qt::AlignCenter);
        if(i < 2) {
            status->setStyleSheet("QLabel { background-color: #dcfce7; color: #166534; border-radius: 12px; font-weight: bold; font-size: 11px; padding: 0px 4px; }");
        } else {
            status->setStyleSheet("QLabel { background-color: #fef3c7; color: #9a3412; border-radius: 12px; font-weight: bold; font-size: 10px; padding: 0px 2px; }");
        }

        QWidget *statusContainer = new QWidget();
        QHBoxLayout *statusLayout = new QHBoxLayout(statusContainer);
        statusLayout->setContentsMargins(2, 2, 2, 2);
        statusLayout->setAlignment(Qt::AlignCenter);
        status->setFixedSize(95, 24);
        statusLayout->addWidget(status);

        table->setCellWidget(i, 6, statusContainer);
        table->setCellWidget(i, 7, createActionButtonsWidget(i));
    }

    tcLayout->addWidget(table);

    // PAGINATION
    paginationLayout = new QHBoxLayout();
    paginationLayout->addStretch();
    tcLayout->addLayout(paginationLayout);

    middleLayout->addWidget(tableCard, 3);

    // ACTIONS RAPIDES
    QFrame *actionsCard = new QFrame();
    actionsCard->setObjectName("card");
    QVBoxLayout *acLayout = new QVBoxLayout(actionsCard);

    QLabel *actionsTitle = new QLabel("Actions rapides");
    actionsTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #0f172a; margin-bottom: 10px;");
    acLayout->addWidget(actionsTitle);

    const QStringList quickActions = {
        "👥  Affecter à une équipe  →",
        "✉️  Envoyer invitation mentor  →",
        "📩  Rappeler aux retardataires  →",
        "🎯  Valider statut projet  →",
        "📊  Générer rapport de progression  →",
        "📜  Exporter Certificat de Participation  →"
    };

    for(const QString &act : quickActions) {
        QPushButton *actBtn = new QPushButton(act);
        actBtn->setObjectName("actionBtn");
        actBtn->setCursor(Qt::PointingHandCursor);
        connect(actBtn, &QPushButton::clicked, this, &MainWindow::onActionRapideClicked);
        acLayout->addWidget(actBtn);
    }
    acLayout->addStretch();

    middleLayout->addWidget(actionsCard, 1);
    contentLayout->addLayout(middleLayout);

    // FOOTER (ODD CARDS)
    QHBoxLayout *oddLayout = new QHBoxLayout();
    oddLayout->setSpacing(15);

    struct ODD {
        QString title;
        QString sub;
        QString color;
    };

    const QList<ODD> odds = {
        {"ODD 4 - Éducation de qualité", "Ateliers Techniques...", "#16a34a"},
        {"ODD 8 - Travail décent", "Suivi de la charge...", "#dc2626"},
        {"ODD 9 - Industrie & Innovation", "Développement d'outils...", "#ea580c"},
        {"ODD 17 - Partenariats", "Renforcement des liens...", "#2563eb"}
    };

    for(const ODD &odd : odds) {
        QFrame *oddCard = new QFrame();
        oddCard->setObjectName("card");
        oddCard->setStyleSheet(QString("border-left: 4px solid %1;").arg(odd.color));
        QVBoxLayout *oLayout = new QVBoxLayout(oddCard);
        oLayout->setContentsMargins(10, 8, 10, 8);

        QLabel *lblTitle = new QLabel(odd.title);
        lblTitle->setStyleSheet("font-size: 11px; font-weight: bold; color: #1e293b;");
        QLabel *lblSub = new QLabel(odd.sub);
        lblSub->setStyleSheet("font-size: 10px; color: #64748b;");

        oLayout->addWidget(lblTitle);
        oLayout->addWidget(lblSub);
        oddLayout->addWidget(oddCard);
    }

    contentLayout->addLayout(oddLayout);
    mainLayout->addWidget(contentArea);

    this->setCentralWidget(centralWidget);
    updateCounters();
    showPage(1);
}

void MainWindow::showPage(int page)
{
    if (!table) return;

    int totalRows = table->rowCount();
    int totalPages = (totalRows + itemsPerPage - 1) / itemsPerPage;
    if (totalPages == 0) totalPages = 1;

    if (page < 1) page = 1;
    if (page > totalPages) page = totalPages;

    currentPage = page;

    int startRow = (currentPage - 1) * itemsPerPage;
    int endRow = startRow + itemsPerPage;

    for (int i = 0; i < totalRows; ++i) {
        if (i >= startRow && i < endRow) {
            table->setRowHidden(i, false);
        } else {
            table->setRowHidden(i, true);
        }
    }

    updatePagination();
}

void MainWindow::updatePagination()
{
    if (!paginationLayout) return;

    QLayoutItem *child;
    while ((child = paginationLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }

    paginationLayout->addStretch();

    int totalRows = table ? table->rowCount() : 0;
    int totalPages = (totalRows + itemsPerPage - 1) / itemsPerPage;
    if (totalPages == 0) totalPages = 1;

    QPushButton *btnPrev = new QPushButton("<");
    btnPrev->setCursor(Qt::PointingHandCursor);
    btnPrev->setObjectName("pageBtn");
    btnPrev->setEnabled(currentPage > 1);
    connect(btnPrev, &QPushButton::clicked, this, &MainWindow::onPrevPageClicked);
    paginationLayout->addWidget(btnPrev);

    for (int i = 1; i <= totalPages; ++i) {
        QPushButton *btn = new QPushButton(QString::number(i));
        btn->setCursor(Qt::PointingHandCursor);

        if (i == currentPage) {
            btn->setObjectName("pageActive");
        } else {
            btn->setObjectName("pageBtn");
        }

        connect(btn, &QPushButton::clicked, this, &MainWindow::onPageClicked);
        paginationLayout->addWidget(btn);
    }

    QPushButton *btnNext = new QPushButton(">");
    btnNext->setCursor(Qt::PointingHandCursor);
    btnNext->setObjectName("pageBtn");
    btnNext->setEnabled(currentPage < totalPages);
    connect(btnNext, &QPushButton::clicked, this, &MainWindow::onNextPageClicked);
    paginationLayout->addWidget(btnNext);

    applyStyleSheet();
}

void MainWindow::onPageClicked()
{
    QPushButton *clickedBtn = qobject_cast<QPushButton*>(sender());
    if (!clickedBtn) return;

    bool ok;
    int page = clickedBtn->text().toInt(&ok);
    if (ok) {
        showPage(page);
    }
}

void MainWindow::onPrevPageClicked()
{
    if (currentPage > 1) {
        showPage(currentPage - 1);
    }
}

void MainWindow::onNextPageClicked()
{
    int totalRows = table ? table->rowCount() : 0;
    int totalPages = (totalRows + itemsPerPage - 1) / itemsPerPage;
    if (currentPage < totalPages) {
        showPage(currentPage + 1);
    }
}

void MainWindow::onAjouterParticipantClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Ajouter un Participant");
    dialog.resize(400, 300);

    QFormLayout form(&dialog);

    QLineEdit *nomInput = new QLineEdit(&dialog);
    QLineEdit *emailInput = new QLineEdit(&dialog);
    QLineEdit *univInput = new QLineEdit(&dialog);
    QLineEdit *compInput = new QLineEdit(&dialog);

    form.addRow("Nom Complet :", nomInput);
    form.addRow("E-mail :", emailInput);
    form.addRow("Université :", univInput);
    form.addRow("Compétence :", compInput);

    QPushButton *submitBtn = new QPushButton("Ajouter", &dialog);
    submitBtn->setStyleSheet("background-color: #2563eb; color: white; padding: 8px; font-weight: bold; border-radius: 6px;");
    form.addWidget(submitBtn);

    connect(submitBtn, &QPushButton::clicked, this, [this, &dialog, nomInput, emailInput, univInput, compInput]() {
        if(nomInput->text().isEmpty() || emailInput->text().isEmpty()) {
            QMessageBox::warning(&dialog, "Attention", "Veuillez remplir au moins le nom et l'email !");
            return;
        }

        int rowCount = table->rowCount();
        table->insertRow(rowCount);

        table->setItem(rowCount, 0, new QTableWidgetItem(QString("PA-00%1").arg(rowCount + 1)));
        table->setItem(rowCount, 1, new QTableWidgetItem(nomInput->text()));
        table->setItem(rowCount, 2, new QTableWidgetItem(emailInput->text()));
        table->setItem(rowCount, 3, new QTableWidgetItem(univInput->text().isEmpty() ? "INSAT" : univInput->text()));
        table->setItem(rowCount, 4, new QTableWidgetItem(compInput->text().isEmpty() ? "Dev" : compInput->text()));
        table->setItem(rowCount, 5, new QTableWidgetItem("Non affecté"));

        QLabel *status = new QLabel("In Progress");
        status->setAlignment(Qt::AlignCenter);
        status->setStyleSheet("QLabel { background-color: #fef3c7; color: #9a3412; border-radius: 12px; font-weight: bold; font-size: 10px; padding: 0px 2px; }");

        QWidget *statusContainer = new QWidget();
        QHBoxLayout *statusLayout = new QHBoxLayout(statusContainer);
        statusLayout->setContentsMargins(2, 2, 2, 2);
        statusLayout->setAlignment(Qt::AlignCenter);
        status->setFixedSize(95, 24);
        statusLayout->addWidget(status);

        table->setCellWidget(rowCount, 6, statusContainer);
        table->setCellWidget(rowCount, 7, createActionButtonsWidget(rowCount));

        nouveauxInscritsCount++;
        updateCounters();

        int totalPages = (table->rowCount() + itemsPerPage - 1) / itemsPerPage;
        showPage(totalPages);

        QMessageBox::information(this, "Succès", "Participant ajouté avec succès !");
        dialog.accept();
    });

    dialog.exec();
}

void MainWindow::onVoirParticipantClicked(int row)
{
    if (!table || row < 0 || row >= table->rowCount()) return;

    QString id = table->item(row, 0) ? table->item(row, 0)->text() : "";
    QString nom = table->item(row, 1) ? table->item(row, 1)->text() : "";
    QString email = table->item(row, 2) ? table->item(row, 2)->text() : "";
    QString univ = table->item(row, 3) ? table->item(row, 3)->text() : "";
    QString comp = table->item(row, 4) ? table->item(row, 4)->text() : "";
    QString equipe = table->item(row, 5) ? table->item(row, 5)->text() : "";

    QString details = QString("<b>ID:</b> %1<br>"
                              "<b>Nom:</b> %2<br>"
                              "<b>Email:</b> %3<br>"
                              "<b>Université:</b> %4<br>"
                              "<b>Compétence:</b> %5<br>"
                              "<b>Équipe:</b> %6")
                          .arg(id, nom, email, univ, comp, equipe);

    QMessageBox::information(this, "Détails du Participant", details);
}

void MainWindow::onModifierParticipantClicked(int row)
{
    if (!table || row < 0 || row >= table->rowCount()) return;

    QDialog dialog(this);
    dialog.setWindowTitle("Modifier le Participant");
    dialog.resize(400, 300);

    QFormLayout form(&dialog);

    QLineEdit *nomInput = new QLineEdit(table->item(row, 1) ? table->item(row, 1)->text() : "", &dialog);
    QLineEdit *emailInput = new QLineEdit(table->item(row, 2) ? table->item(row, 2)->text() : "", &dialog);
    QLineEdit *univInput = new QLineEdit(table->item(row, 3) ? table->item(row, 3)->text() : "", &dialog);
    QLineEdit *compInput = new QLineEdit(table->item(row, 4) ? table->item(row, 4)->text() : "", &dialog);

    form.addRow("Nom Complet :", nomInput);
    form.addRow("E-mail :", emailInput);
    form.addRow("Université :", univInput);
    form.addRow("Compétence :", compInput);

    QPushButton *submitBtn = new QPushButton("Enregistrer", &dialog);
    submitBtn->setStyleSheet("background-color: #2563eb; color: white; padding: 8px; font-weight: bold; border-radius: 6px;");
    form.addWidget(submitBtn);

    connect(submitBtn, &QPushButton::clicked, this, [this, &dialog, row, nomInput, emailInput, univInput, compInput]() {
        if(table->item(row, 1)) table->item(row, 1)->setText(nomInput->text());
        if(table->item(row, 2)) table->item(row, 2)->setText(emailInput->text());
        if(table->item(row, 3)) table->item(row, 3)->setText(univInput->text());
        if(table->item(row, 4)) table->item(row, 4)->setText(compInput->text());

        QMessageBox::information(this, "Succès", "Informations mises à jour !");
        dialog.accept();
    });

    dialog.exec();
}

void MainWindow::onSupprimerParticipantClicked(int row)
{
    if (!table || row < 0 || row >= table->rowCount()) return;

    QString nom = table->item(row, 1) ? table->item(row, 1)->text() : "";

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation",
                                  QString("Voulez-vous vraiment supprimer %1 ?").arg(nom),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        table->removeRow(row);
        updateCounters();

        for(int i = 0; i < table->rowCount(); ++i) {
            table->setCellWidget(i, 7, createActionButtonsWidget(i));
        }

        showPage(currentPage);

        QMessageBox::information(this, "Supprimé", "Participant supprimé avec succès !");
    }
}

void MainWindow::onActionRapideClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    QString text = btn->text();

    if(text.contains("Affecter à une équipe")) {
        int currentRow = table->currentRow();
        if(currentRow < 0) {
            QMessageBox::warning(this, "Sélection requise", "Veuillez sélectionner un participant dans le tableau d'abord !");
            return;
        }
        if(table->item(currentRow, 5)) table->item(currentRow, 5)->setText("Team Vision...");
        QMessageBox::information(this, "Équipe", "Le participant a été affecté à l'équipe 'Team Vision...' !");
    }
    else if(text.contains("Envoyer invitation mentor")) {
        QMessageBox::information(this, "Invitation", "Invitation envoyée aux mentors par e-mail !");
    }
    else if(text.contains("Rappeler aux retardataires")) {
        QMessageBox::information(this, "Rappel", "Rappel envoyé aux participants avec statut 'In Progress' !");
    }
    else if(text.contains("Valider statut projet")) {
        int currentRow = table->currentRow();
        if(currentRow >= 0) {
            QLabel *status = new QLabel("Done");
            status->setAlignment(Qt::AlignCenter);
            status->setStyleSheet("QLabel { background-color: #dcfce7; color: #166534; border-radius: 12px; font-weight: bold; font-size: 11px; padding: 0px 4px; }");

            QWidget *statusContainer = new QWidget();
            QHBoxLayout *statusLayout = new QHBoxLayout(statusContainer);
            statusLayout->setContentsMargins(2, 2, 2, 2);
            statusLayout->setAlignment(Qt::AlignCenter);
            status->setFixedSize(95, 24);
            statusLayout->addWidget(status);

            table->setCellWidget(currentRow, 6, statusContainer);
            QMessageBox::information(this, "Statut", "Statut changé en 'Done' !");
        } else {
            QMessageBox::information(this, "Statut", "Statut des projets validé pour l'ensemble des groupes !");
        }
    }
    else if(text.contains("Générer rapport")) {
        QMessageBox::information(this, "Rapport", "Rapport de progression généré avec succès (PDF) !");
    }
    else if(text.contains("Exporter Certificat")) {
        QMessageBox::information(this, "Certificat", "Certificat de participation exporté avec succès !");
    }
}

void MainWindow::applyStyleSheet()
{
    QString style = R"(
        QMainWindow { background-color: #f8fafc; }
        #sidebar { background-color: #0d1b2a; }
        #menuBtn { background: transparent; color: #94a3b8; text-align: left; padding: 10px 15px; border: none; border-radius: 8px; font-size: 13px; font-weight: 500; }
        #menuBtn:hover { background-color: #1e293b; color: #f8fafc; }
        #activeBtn { background-color: #2563eb; color: white; text-align: left; padding: 10px 15px; border-radius: 8px; font-size: 13px; font-weight: bold; }
        #card { background-color: #ffffff; border-radius: 12px; padding: 15px; border: 1px solid #e2e8f0; }
        #primaryBtn { background-color: #2563eb; color: white; padding: 8px 16px; border-radius: 8px; font-weight: bold; border: none; }
        #primaryBtn:hover { background-color: #1d4ed8; }
        #actionBtn { background-color: #f8fafc; border: 1px solid #e2e8f0; padding: 10px 12px; border-radius: 8px; text-align: left; color: #334155; font-size: 12px; font-weight: 500; }
        #actionBtn:hover { background-color: #f1f5f9; border-color: #cbd5e1; }
        #pageBtn { background-color: #f1f5f9; border: 1px solid #e2e8f0; border-radius: 6px; padding: 5px 10px; color: #475569; }
        #pageBtn:disabled { background-color: #f8fafc; color: #cbd5e1; border-color: #f1f5f9; }
        #pageActive { background-color: #2563eb; color: white; border-radius: 6px; padding: 5px 10px; font-weight: bold; border: none; }
        QTableWidget { border: none; background: transparent; gridline-color: transparent; }
        QTableWidget::item { padding: 8px; color: #334155; border-bottom: 1px solid #f1f5f9; }
        QHeaderView::section { background-color: #f8fafc; border: none; padding: 8px; font-weight: bold; color: #64748b; font-size: 12px; }
    )";

    this->setStyleSheet(style);
}