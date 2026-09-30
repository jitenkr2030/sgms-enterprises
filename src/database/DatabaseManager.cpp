#include <QCoreApplication>
#include <QSqlRecord>
#include "DatabaseManager.h"

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager() = default;

DatabaseManager::~DatabaseManager()
{
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::initialize(const QString& dbPath)
{
    if (dbPath.isEmpty()) {
        QString appDir = QCoreApplication::applicationDirPath();
        m_dbPath = appDir + "/database.db";
    } else {
        m_dbPath = dbPath;
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(m_dbPath);

    if (!m_db.open()) {
        qCritical() << "Failed to open database:" << m_db.lastError().text();
        return false;
    }

    // Enable WAL mode for better concurrent performance
    QSqlQuery pragma(m_db);
    pragma.exec("PRAGMA journal_mode=WAL");
    pragma.exec("PRAGMA foreign_keys=ON");

    createTables();
    seedDefaultAdmin();
    seedDefaultPermissions();

    qInfo() << "Database initialized at:" << m_dbPath;
    return true;
}

bool DatabaseManager::isOpen() const
{
    return m_db.isOpen();
}

QString DatabaseManager::databasePath() const
{
    return m_dbPath;
}

QSqlQuery DatabaseManager::execute(const QString& sql, const QVariantMap& params)
{
    QSqlQuery query(m_db);
    query.prepare(sql);

    for (auto it = params.begin(); it != params.end(); ++it) {
        query.bindValue(it.key(), it.value());
    }

    if (!query.exec()) {
        qWarning() << "SQL Error:" << query.lastError().text();
        qWarning() << "Query:" << sql;
    }

    return query;
}

bool DatabaseManager::executeNonQuery(const QString& sql, const QVariantMap& params)
{
    auto query = execute(sql, params);
    return query.lastError().type() == QSqlError::NoError;
}

int DatabaseManager::lastInsertId() const
{
    QSqlQuery query(m_db);
    query.exec("SELECT last_insert_rowid()");
    if (query.next())
        return query.value(0).toInt();
    return -1;
}

bool DatabaseManager::tableExists(const QString& tableName) const
{
    QSqlQuery query(m_db);
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=:name");
    query.bindValue(":name", tableName);
    query.exec();
    return query.next();
}

// ═══════════════════════════════════════════════════════════════
//  TABLE CREATION
// ═══════════════════════════════════════════════════════════════

void DatabaseManager::createTables()
{
    // Industry settings table
    QSqlQuery settingsQuery(m_db);
    settingsQuery.exec("CREATE TABLE IF NOT EXISTS app_settings ("
                       "key TEXT PRIMARY KEY,"
                       "value TEXT"
                       ")");
    // License machine tracking (2-PC limit)
    settingsQuery.exec("CREATE TABLE IF NOT EXISTS license_machines ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "license_key TEXT NOT NULL,"
                       "machine_id TEXT NOT NULL,"
                       "activated_at TEXT DEFAULT (datetime('now','localtime')),"
                       "UNIQUE(license_key, machine_id)"
                       ")");

    QStringList statements;

    // Users
    statements << R"(
        CREATE TABLE IF NOT EXISTS Users (
            id            INTEGER PRIMARY KEY AUTOINCREMENT,
            username      TEXT    NOT NULL UNIQUE,
            password_hash TEXT    NOT NULL,
            full_name     TEXT    NOT NULL,
            role          TEXT    NOT NULL DEFAULT 'staff',
            is_active     INTEGER NOT NULL DEFAULT 1,
            created_at    TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
            updated_at    TEXT    NOT NULL DEFAULT (datetime('now','localtime'))
        )
    )";

    // Guards
    statements << R"(
        CREATE TABLE IF NOT EXISTS Guards (
            id                   INTEGER PRIMARY KEY AUTOINCREMENT,
            guard_code           TEXT    NOT NULL UNIQUE,
            full_name            TEXT    NOT NULL,
            father_name          TEXT    DEFAULT '',
            date_of_birth        TEXT    DEFAULT '',
            gender               TEXT    DEFAULT 'Male',
            marital_status       TEXT    DEFAULT '',
            mobile_primary       TEXT    DEFAULT '',
            mobile_secondary     TEXT    DEFAULT '',
            email                TEXT    DEFAULT '',
            address_line1        TEXT    DEFAULT '',
            address_line2        TEXT    DEFAULT '',
            city                 TEXT    DEFAULT '',
            state                TEXT    DEFAULT '',
            pincode              TEXT    DEFAULT '',
            aadhaar_number       TEXT    DEFAULT '',
            pan_number           TEXT    DEFAULT '',
            photo_path           TEXT    DEFAULT '',
            police_verified      INTEGER NOT NULL DEFAULT 0,
            police_verify_date   TEXT    DEFAULT '',
            joining_date         TEXT    NOT NULL,
            termination_date     TEXT    DEFAULT '',
            status               TEXT    NOT NULL DEFAULT 'Active',
            bank_name            TEXT    DEFAULT '',
            bank_account         TEXT    DEFAULT '',
    )"
    "   bank_ifsc            TEXT    DEFAULT '',"
    R"(
            uan_number           TEXT    DEFAULT '',
            esic_number          TEXT    DEFAULT '',
            pf_number            TEXT    DEFAULT '',
            basic_salary         REAL    NOT NULL DEFAULT 0,
            hra                  REAL    NOT NULL DEFAULT 0,
            conveyance           REAL    NOT NULL DEFAULT 0,
            medical_allowance    REAL    NOT NULL DEFAULT 0,
            special_allowance    REAL    NOT NULL DEFAULT 0,
            site_id              INTEGER DEFAULT NULL,
            client_id            INTEGER DEFAULT NULL,
            notes                TEXT    DEFAULT '',
            created_at           TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
            updated_at           TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
            FOREIGN KEY (site_id)   REFERENCES Sites(id) ON DELETE SET NULL,
            FOREIGN KEY (client_id) REFERENCES Clients(id) ON DELETE SET NULL
        )
    )";

    // Clients
    statements << R"(
        CREATE TABLE IF NOT EXISTS Clients (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            client_code     TEXT    NOT NULL UNIQUE,
            company_name    TEXT    NOT NULL,
            contact_person  TEXT    DEFAULT '',
            mobile          TEXT    DEFAULT '',
            email           TEXT    DEFAULT '',
            address         TEXT    DEFAULT '',
            city            TEXT    DEFAULT '',
            gst_number      TEXT    DEFAULT '',
            billing_rate    REAL    DEFAULT 0,
            contract_start  TEXT    DEFAULT '',
            contract_end    TEXT    DEFAULT '',
            agreement_path  TEXT    DEFAULT '',
            invoice_terms   TEXT    DEFAULT '',
            status          TEXT    NOT NULL DEFAULT 'Active',
            notes           TEXT    DEFAULT '',
            created_at      TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
            updated_at      TEXT    NOT NULL DEFAULT (datetime('now','localtime'))
        )
    )";

    // Sites
    statements << R"(
        CREATE TABLE IF NOT EXISTS Sites (
                id                INTEGER PRIMARY KEY AUTOINCREMENT,
                site_code         TEXT    NOT NULL UNIQUE,
                site_name         TEXT    NOT NULL,
                address           TEXT    DEFAULT '',
                city              TEXT    DEFAULT '',
                client_id         INTEGER,
                shift_morning     TEXT    DEFAULT '06:00-14:00',
                shift_afternoon   TEXT    DEFAULT '14:00-22:00',
                shift_night       TEXT    DEFAULT '22:00-06:00',
                supervisor_id     INTEGER DEFAULT NULL,
                guards_required   INTEGER NOT NULL DEFAULT 1,
                site_instructions TEXT    DEFAULT '',
                status            TEXT    NOT NULL DEFAULT 'Active',
                created_at        TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
                updated_at        TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
                FOREIGN KEY (client_id)     REFERENCES Clients(id) ON DELETE SET NULL,
                FOREIGN KEY (supervisor_id) REFERENCES Guards(id)  ON DELETE SET NULL
            )
    )";

    // Attendance
    statements << R"(
        CREATE TABLE IF NOT EXISTS Attendance (
                id          INTEGER PRIMARY KEY AUTOINCREMENT,
                guard_id    INTEGER NOT NULL,
                site_id     INTEGER NOT NULL,
                date        TEXT    NOT NULL,
                status      TEXT    NOT NULL DEFAULT 'Present',
                check_in    TEXT    DEFAULT '',
                check_out   TEXT    DEFAULT '',
                hours_worked REAL   DEFAULT 0,
                overtime    REAL    DEFAULT 0,
                late_entry  INTEGER NOT NULL DEFAULT 0,
                notes       TEXT    DEFAULT '',
                marked_by   INTEGER DEFAULT NULL,
                created_at  TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
                FOREIGN KEY (guard_id)  REFERENCES Guards(id) ON DELETE CASCADE,
                FOREIGN KEY (site_id)   REFERENCES Sites(id)  ON DELETE CASCADE,
                FOREIGN KEY (marked_by) REFERENCES Users(id)  ON DELETE SET NULL,
                UNIQUE(guard_id, date)
            )
    )";

    // Duty Allocation
    statements << R"(
        CREATE TABLE IF NOT EXISTS Duty (
                id          INTEGER PRIMARY KEY AUTOINCREMENT,
                guard_id    INTEGER NOT NULL,
                site_id     INTEGER NOT NULL,
                shift       TEXT    NOT NULL DEFAULT 'Morning',
                start_date  TEXT    NOT NULL,
                end_date    TEXT    DEFAULT '',
                is_permanent INTEGER NOT NULL DEFAULT 1,
                notes       TEXT    DEFAULT '',
                created_at  TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
                FOREIGN KEY (guard_id) REFERENCES Guards(id) ON DELETE CASCADE,
                FOREIGN KEY (site_id)  REFERENCES Sites(id)  ON DELETE CASCADE
            )
    )";

    // Salary
    statements << R"(
        CREATE TABLE IF NOT EXISTS Salary (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                guard_id        INTEGER NOT NULL,
                month           INTEGER NOT NULL,
                year            INTEGER NOT NULL,
                working_days    INTEGER NOT NULL DEFAULT 0,
                present_days    INTEGER NOT NULL DEFAULT 0,
                absent_days     INTEGER NOT NULL DEFAULT 0,
                leave_days      INTEGER NOT NULL DEFAULT 0,
                overtime_hours  REAL    NOT NULL DEFAULT 0,
                basic_salary    REAL    NOT NULL DEFAULT 0,
                hra             REAL    NOT NULL DEFAULT 0,
                conveyance      REAL    NOT NULL DEFAULT 0,
                medical         REAL    NOT NULL DEFAULT 0,
                special         REAL    NOT NULL DEFAULT 0,
                overtime_pay    REAL    NOT NULL DEFAULT 0,
                bonus           REAL    NOT NULL DEFAULT 0,
                gross_salary    REAL    NOT NULL DEFAULT 0,
                pf_deduction    REAL    NOT NULL DEFAULT 0,
                esic_deduction  REAL    NOT NULL DEFAULT 0,
                pt_deduction    REAL    NOT NULL DEFAULT 0,
                advance         REAL    NOT NULL DEFAULT 0,
                penalty         REAL    NOT NULL DEFAULT 0,
                total_deduction REAL    NOT NULL DEFAULT 0,
                net_salary      REAL    NOT NULL DEFAULT 0,
                payment_status  TEXT    NOT NULL DEFAULT 'Pending',
                payment_date    TEXT    DEFAULT '',
                payment_mode    TEXT    DEFAULT '',
                notes           TEXT    DEFAULT '',
                created_at      TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
                FOREIGN KEY (guard_id) REFERENCES Guards(id) ON DELETE CASCADE,
                UNIQUE(guard_id, month, year)
            )
    )";

    // Leave
    statements << R"(
        CREATE TABLE IF NOT EXISTS LeaveRecord (
                id          INTEGER PRIMARY KEY AUTOINCREMENT,
                guard_id    INTEGER NOT NULL,
                leave_type  TEXT    NOT NULL DEFAULT 'Casual',
                start_date  TEXT    NOT NULL,
                end_date    TEXT    NOT NULL,
                days        INTEGER NOT NULL DEFAULT 1,
                reason      TEXT    DEFAULT '',
                status      TEXT    NOT NULL DEFAULT 'Pending',
                approved_by INTEGER DEFAULT NULL,
                created_at  TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
                FOREIGN KEY (guard_id)    REFERENCES Guards(id) ON DELETE CASCADE,
                FOREIGN KEY (approved_by) REFERENCES Users(id)  ON DELETE SET NULL
            )
    )";

    // Incidents
    statements << R"(
        CREATE TABLE IF NOT EXISTS Incidents (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                incident_code   TEXT    NOT NULL UNIQUE,
                incident_type   TEXT    NOT NULL,
                severity        TEXT    NOT NULL DEFAULT 'Medium',
                site_id         INTEGER,
                guard_id        INTEGER,
                date_time       TEXT    NOT NULL,
                description     TEXT    NOT NULL,
                action_taken    TEXT    DEFAULT '',
                reported_by     TEXT    DEFAULT '',
                witness_name    TEXT    DEFAULT '',
                witness_contact TEXT    DEFAULT '',
                photo_path      TEXT    DEFAULT '',
                document_path   TEXT    DEFAULT '',
                status          TEXT    NOT NULL DEFAULT 'Open',
                resolution      TEXT    DEFAULT '',
                created_at      TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
                FOREIGN KEY (site_id)  REFERENCES Sites(id)  ON DELETE SET NULL,
                FOREIGN KEY (guard_id) REFERENCES Guards(id) ON DELETE SET NULL
            )
    )";

    // Visitors
    statements << R"(
        CREATE TABLE IF NOT EXISTS Visitors (
                id          INTEGER PRIMARY KEY AUTOINCREMENT,
                site_id     INTEGER NOT NULL,
                name        TEXT    NOT NULL,
                mobile      TEXT    DEFAULT '',
                id_proof    TEXT    DEFAULT '',
                id_number   TEXT    DEFAULT '',
                purpose     TEXT    DEFAULT '',
                whom_to_meet TEXT   DEFAULT '',
                entry_time  TEXT    NOT NULL,
                exit_time   TEXT    DEFAULT '',
                photo_path  TEXT    DEFAULT '',
                vehicle_no  TEXT    DEFAULT '',
                notes       TEXT    DEFAULT '',
                created_at  TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
                FOREIGN KEY (site_id) REFERENCES Sites(id) ON DELETE CASCADE
            )
    )";

    // Vehicles
    statements << R"(
        CREATE TABLE IF NOT EXISTS Vehicles (
                id          INTEGER PRIMARY KEY AUTOINCREMENT,
                site_id     INTEGER NOT NULL,
                vehicle_no  TEXT    NOT NULL,
                vehicle_type TEXT   DEFAULT '',
                driver_name TEXT    DEFAULT '',
                driver_mobile TEXT  DEFAULT '',
                purpose     TEXT    DEFAULT '',
                entry_time  TEXT    NOT NULL,
                exit_time   TEXT    DEFAULT '',
                notes       TEXT    DEFAULT '',
                created_at  TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
                FOREIGN KEY (site_id) REFERENCES Sites(id) ON DELETE CASCADE
            )
    )";

    // Equipment
    statements << R"(
        CREATE TABLE IF NOT EXISTS Equipment (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                equipment_code  TEXT    NOT NULL UNIQUE,
                equipment_type  TEXT    NOT NULL,
                description     TEXT    DEFAULT '',
                serial_number   TEXT    DEFAULT '',
                purchase_date   TEXT    DEFAULT '',
                condition       TEXT    NOT NULL DEFAULT 'Good',
                status          TEXT    NOT NULL DEFAULT 'Available',
                notes           TEXT    DEFAULT '',
                created_at      TEXT    NOT NULL DEFAULT (datetime('now','localtime'))
            )
    )";

    // Equipment Issue Log
    statements << R"(
        CREATE TABLE IF NOT EXISTS EquipmentIssue (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                equipment_id    INTEGER NOT NULL,
                guard_id        INTEGER NOT NULL,
                issue_date      TEXT    NOT NULL,
                return_date     TEXT    DEFAULT '',
                condition_out   TEXT    DEFAULT 'Good',
                condition_in    TEXT    DEFAULT '',
                notes           TEXT    DEFAULT '',
                created_at      TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
                FOREIGN KEY (equipment_id) REFERENCES Equipment(id) ON DELETE CASCADE,
                FOREIGN KEY (guard_id)     REFERENCES Guards(id)   ON DELETE CASCADE
            )
    )";

    // Uniform
    statements << R"(
        CREATE TABLE IF NOT EXISTS Uniform (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                item_type       TEXT    NOT NULL,
                size            TEXT    DEFAULT '',
                quantity        INTEGER NOT NULL DEFAULT 0,
                status          TEXT    NOT NULL DEFAULT 'Available',
                notes           TEXT    DEFAULT '',
                created_at      TEXT    NOT NULL DEFAULT (datetime('now','localtime'))
            )
    )";

    // Uniform Issue Log
    statements << R"(
        CREATE TABLE IF NOT EXISTS UniformIssue (
                id          INTEGER PRIMARY KEY AUTOINCREMENT,
                uniform_id  INTEGER NOT NULL,
                guard_id    INTEGER NOT NULL,
                issue_date  TEXT    NOT NULL,
                return_date TEXT    DEFAULT '',
                condition_out TEXT  DEFAULT 'New',
                condition_in  TEXT  DEFAULT '',
                notes       TEXT    DEFAULT '',
                created_at  TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
                FOREIGN KEY (uniform_id) REFERENCES Uniform(id) ON DELETE CASCADE,
                FOREIGN KEY (guard_id)   REFERENCES Guards(id)  ON DELETE CASCADE
            )
    )";

    // Training
    statements << R"(
        CREATE TABLE IF NOT EXISTS Training (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                training_name   TEXT    NOT NULL,
                training_type   TEXT    DEFAULT '',
                description     TEXT    DEFAULT '',
                trainer_name    TEXT    DEFAULT '',
                start_date      TEXT    NOT NULL,
                end_date        TEXT    DEFAULT '',
                location        TEXT    DEFAULT '',
                notes           TEXT    DEFAULT '',
                created_at      TEXT    NOT NULL DEFAULT (datetime('now','localtime'))
            )
    )";

    // Training Participants
    statements << R"(
        CREATE TABLE IF NOT EXISTS TrainingParticipant (
                id          INTEGER PRIMARY KEY AUTOINCREMENT,
                training_id INTEGER NOT NULL,
                guard_id    INTEGER NOT NULL,
                attendance  TEXT    DEFAULT 'Present',
                score       REAL    DEFAULT 0,
                certificate TEXT    DEFAULT '',
                FOREIGN KEY (training_id) REFERENCES Training(id) ON DELETE CASCADE,
                FOREIGN KEY (guard_id)    REFERENCES Guards(id)   ON DELETE CASCADE
            )
    )";

    // Documents
    statements << R"(
        CREATE TABLE IF NOT EXISTS Documents (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                guard_id        INTEGER,
                client_id       INTEGER,
                document_type   TEXT    NOT NULL,
                file_name       TEXT    NOT NULL,
                file_path       TEXT    NOT NULL,
                description     TEXT    DEFAULT '',
                created_at      TEXT    NOT NULL DEFAULT (datetime('now','localtime')),
                FOREIGN KEY (guard_id)  REFERENCES Guards(id)  ON DELETE CASCADE,
                FOREIGN KEY (client_id) REFERENCES Clients(id) ON DELETE CASCADE
            )
    )";

    // Settings
    statements << R"(
        CREATE TABLE IF NOT EXISTS Settings (
                id      INTEGER PRIMARY KEY AUTOINCREMENT,
                key     TEXT NOT NULL UNIQUE,
                value   TEXT DEFAULT '',
                updated_at TEXT NOT NULL DEFAULT (datetime('now','localtime'))
            )
    )";



    // Compliance - Licenses
    statements << R"(
        CREATE TABLE IF NOT EXISTS ComplianceLicenses (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                license_type    TEXT NOT NULL,
                license_number  TEXT NOT NULL,
                issuing_authority TEXT,
                state           TEXT,
                issue_date      TEXT,
                expiry_date     TEXT,
                renewal_status  TEXT DEFAULT 'Active',
                document_path   TEXT,
                notes           TEXT,
                status          TEXT DEFAULT 'Active',
                created_at      TEXT DEFAULT (datetime('now','localtime'))
            )
    )""";

    // Compliance - Filings
    statements << R"(
        CREATE TABLE IF NOT EXISTS ComplianceFilings (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                compliance_area TEXT NOT NULL,
                filing_type     TEXT NOT NULL,
                filing_period   TEXT,
                due_date        TEXT NOT NULL,
                filed_date      TEXT,
                amount          REAL DEFAULT 0,
                status          TEXT DEFAULT 'Pending',
                challan_path    TEXT,
                notes           TEXT,
                created_at      TEXT DEFAULT (datetime('now','localtime'))
            )
    )""";

    // Compliance - Minimum Wages
    statements << R"(
        CREATE TABLE IF NOT EXISTS MinWages (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                state           TEXT NOT NULL,
                zone            TEXT DEFAULT 'Zone I',
                skill_category  TEXT DEFAULT 'Unskilled',
                basic_wage      REAL NOT NULL,
                vda             REAL DEFAULT 0,
                total_wage      REAL NOT NULL,
                effective_from  TEXT,
                effective_to    TEXT,
                notification_no TEXT,
                notes           TEXT,
                created_at      TEXT DEFAULT (datetime('now','localtime'))
            )
    )""";

    // Compliance - Config
    statements << R"(
        CREATE TABLE IF NOT EXISTS ComplianceConfig (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                compliance_area TEXT NOT NULL,
                config_key      TEXT NOT NULL,
                config_value    TEXT,
                state           TEXT,
                effective_from  TEXT,
                notes           TEXT,
                UNIQUE(compliance_area, config_key, state)
            )
    )""";

    // Role Permissions
    statements << R"(
        CREATE TABLE IF NOT EXISTS RolePermissions (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                role_name       TEXT NOT NULL,
                module_name     TEXT NOT NULL,
                can_view        INTEGER DEFAULT 1,
                can_create      INTEGER DEFAULT 1,
                can_edit        INTEGER DEFAULT 1,
                can_delete      INTEGER DEFAULT 0,
                UNIQUE(role_name, module_name)
            )
    )";

    // Complaints / Feedback
    statements << R"(
        CREATE TABLE IF NOT EXISTS Complaints (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                complaint_code  TEXT NOT NULL UNIQUE,
                complaint_type  TEXT NOT NULL DEFAULT 'Client',
                category        TEXT DEFAULT 'General',
                source          TEXT DEFAULT 'Client',
                client_id       INTEGER,
                site_id         INTEGER,
                guard_id        INTEGER,
                complainant_name TEXT,
                complainant_contact TEXT,
                subject         TEXT NOT NULL,
                description     TEXT,
                severity        TEXT DEFAULT 'Medium',
                status          TEXT DEFAULT 'Open',
                assigned_to     TEXT,
                resolution      TEXT,
                resolved_date   TEXT,
                created_at      TEXT NOT NULL DEFAULT (datetime('now','localtime'))
            )
    )";

    // Fines / Deductions Ledger
    statements << R"(
        CREATE TABLE IF NOT EXISTS Fines (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                guard_id        INTEGER NOT NULL,
                fine_type       TEXT NOT NULL,
                reason          TEXT NOT NULL,
                amount          REAL DEFAULT 0,
                fine_date       TEXT NOT NULL,
                deduction_month INTEGER,
                deduction_year  INTEGER,
                status          TEXT DEFAULT 'Pending',
                approved_by     TEXT,
                notes           TEXT,
                created_at      TEXT NOT NULL DEFAULT (datetime('now','localtime'))
            )
    )";

    // Payroll
    statements << R"(
        CREATE TABLE IF NOT EXISTS Payroll (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                guard_id        INTEGER NOT NULL,
                month           INTEGER NOT NULL,
                year            INTEGER NOT NULL,
                basic_salary    REAL DEFAULT 0,
                hra             REAL DEFAULT 0,
                da              REAL DEFAULT 0,
                overtime_hours  REAL DEFAULT 0,
                overtime_pay    REAL DEFAULT 0,
                other_allowances REAL DEFAULT 0,
                gross_salary    REAL DEFAULT 0,
                pf_deduction    REAL DEFAULT 0,
                esic_deduction  REAL DEFAULT 0,
                pt_deduction    REAL DEFAULT 0,
                advance         REAL DEFAULT 0,
                penalty         REAL DEFAULT 0,
                fines           REAL DEFAULT 0,
                other_deductions REAL DEFAULT 0,
                total_deduction REAL DEFAULT 0,
                net_salary      REAL DEFAULT 0,
                payment_status  TEXT DEFAULT 'Pending',
                payment_date    TEXT,
                payment_mode    TEXT DEFAULT 'Bank Transfer',
                payslip_path    TEXT,
                generated_at    TEXT NOT NULL DEFAULT (datetime('now','localtime'))
            )
    )";

    // Announcements / Circulars
    statements << R"(
        CREATE TABLE IF NOT EXISTS Announcements (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                title           TEXT NOT NULL,
                message         TEXT NOT NULL,
                target_type     TEXT DEFAULT 'All',
                target_id       INTEGER DEFAULT 0,
                priority        TEXT DEFAULT 'Normal',
                published_by    TEXT,
                published_at    TEXT NOT NULL DEFAULT (datetime('now','localtime')),
                expires_at      TEXT,
                status          TEXT DEFAULT 'Active'
            )
    )";

    // Photo Gallery
    statements << R"(
        CREATE TABLE IF NOT EXISTS Photos (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                category        TEXT DEFAULT 'Other',
                title           TEXT,
                file_path       TEXT NOT NULL,
                related_type    TEXT,
                related_id      INTEGER,
                site_id         INTEGER,
                taken_date      TEXT,
                uploaded_by     TEXT,
                notes           TEXT,
                created_at      TEXT NOT NULL DEFAULT (datetime('now','localtime'))
            )
    )";

    // Client Billing / Invoices
    statements << R"(
        CREATE TABLE IF NOT EXISTS Invoices (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                invoice_code    TEXT NOT NULL UNIQUE,
                client_id       INTEGER NOT NULL,
                invoice_month   INTEGER NOT NULL,
                invoice_year    INTEGER NOT NULL,
                site_id         INTEGER,
                guards_deployed INTEGER DEFAULT 0,
                working_days    INTEGER DEFAULT 26,
                per_guard_rate  REAL DEFAULT 0,
                total_guard_charges REAL DEFAULT 0,
                equipment_charges REAL DEFAULT 0,
                other_charges   REAL DEFAULT 0,
                subtotal        REAL DEFAULT 0,
                gst_rate        REAL DEFAULT 18,
                gst_amount      REAL DEFAULT 0,
                total_amount    REAL DEFAULT 0,
                status          TEXT DEFAULT 'Draft',
                invoice_date    TEXT,
                due_date        TEXT,
                payment_date    TEXT,
                notes           TEXT,
                invoice_path    TEXT,
                created_at      TEXT NOT NULL DEFAULT (datetime('now','localtime'))
            )
    )";

    // Helpdesk / Tickets
    statements << R"(
        CREATE TABLE IF NOT EXISTS Tickets (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                ticket_code     TEXT NOT NULL UNIQUE,
                category        TEXT DEFAULT 'General',
                priority        TEXT DEFAULT 'Medium',
                subject         TEXT NOT NULL,
                description     TEXT,
                raised_by       TEXT,
                assigned_to     TEXT,
                status          TEXT DEFAULT 'Open',
                resolution      TEXT,
                resolved_date   TEXT,
                created_at      TEXT NOT NULL DEFAULT (datetime('now','localtime'))
            )
    )";

    // Audit Log
    statements << R"(
        CREATE TABLE IF NOT EXISTS AuditLog (
                id              INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id         INTEGER,
                username        TEXT,
                action          TEXT NOT NULL,
                module          TEXT NOT NULL,
                record_id       INTEGER,
                details         TEXT,
                ip_address      TEXT,
                created_at      TEXT NOT NULL DEFAULT (datetime('now','localtime'))
            )
    )";

    // Backup Log
    statements << R"(
        CREATE TABLE IF NOT EXISTS BackupLog (
                id          INTEGER PRIMARY KEY AUTOINCREMENT,
                backup_path TEXT    NOT NULL,
                backup_type TEXT    NOT NULL DEFAULT 'Manual',
                file_size   INTEGER DEFAULT 0,
                created_at  TEXT    NOT NULL DEFAULT (datetime('now','localtime'))
            )
    )";

    for (const auto& sql : statements) {
        QSqlQuery query(m_db);
        if (!query.exec(sql)) {
            qCritical() << "Table creation error:" << query.lastError().text();
            qCritical() << "SQL:" << sql.left(100);
        }
    }
}

void DatabaseManager::seedDefaultPermissions()
{
    QSqlQuery check(m_db);
    check.exec("SELECT COUNT(*) FROM RolePermissions");
    if (check.next() && check.value(0).toInt() > 0) return;

    QStringList modules = {"Dashboard", "Guards", "Clients", "Sites", "Attendance",
                           "Duty", "Leave", "Salary", "Uniform", "Equipment",
                           "Visitors", "Vehicles", "Incidents", "Training",
                           "Documents", "Complaints", "Fines", "Alerts",
                           "Payroll", "Announcements", "Photos", "Invoices",
                           "Tickets", "AuditLog", "Reports", "Search",
                           "Backup", "Settings"};

    // Admin gets full access
    for (const auto& mod : modules) {
        QSqlQuery q(m_db);
        q.prepare("INSERT INTO RolePermissions (role_name, module_name, can_view, can_create, can_edit, can_delete) VALUES ('Admin', :mod, 1, 1, 1, 1)");
        q.bindValue(":mod", mod);
        q.exec();
    }

    // Supervisor gets access except Settings, Backup, AuditLog, RoleManagement
    QStringList supervisorDeny = {"Backup", "Settings"};
    for (const auto& mod : modules) {
        QSqlQuery q(m_db);
        bool deny = supervisorDeny.contains(mod);
        q.prepare("INSERT INTO RolePermissions (role_name, module_name, can_view, can_create, can_edit, can_delete) VALUES ('Supervisor', :mod, 1, :create, :edit, :del)");
        q.bindValue(":mod", mod);
        q.bindValue(":create", deny ? 0 : 1);
        q.bindValue(":edit", deny ? 0 : 1);
        q.bindValue(":del", deny ? 0 : (mod == "Attendance" || mod == "Duty" || mod == "Leave" ? 1 : 0));
        q.exec();
    }

    // Operator gets limited access
    QStringList operatorAllow = {"Dashboard", "Guards", "Clients", "Sites", "Attendance",
                                  "Duty", "Leave", "Visitors", "Vehicles", "Incidents",
                                  "Documents", "Complaints", "Alerts"};
    QStringList operatorCreate = {"Attendance", "Leave", "Visitors", "Vehicles", "Incidents", "Complaints"};
    QStringList operatorEdit = {"Attendance", "Leave", "Visitors", "Vehicles"};
    for (const auto& mod : modules) {
        QSqlQuery q(m_db);
        bool allow = operatorAllow.contains(mod);
        q.prepare("INSERT INTO RolePermissions (role_name, module_name, can_view, can_create, can_edit, can_delete) VALUES ('Operator', :mod, :view, :create, :edit, 0)");
        q.bindValue(":mod", mod);
        q.bindValue(":view", allow ? 1 : 0);
        q.bindValue(":create", operatorCreate.contains(mod) ? 1 : 0);
        q.bindValue(":edit", operatorEdit.contains(mod) ? 1 : 0);
        q.exec();
    }
}

bool DatabaseManager::hasPermission(const QString& role, const QString& module, const QString& action)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT " + action + " FROM RolePermissions WHERE role_name = :role AND module_name = :mod");
    q.bindValue(":role", role);
    q.bindValue(":mod", module);
    if (q.exec() && q.next()) return q.value(0).toInt() == 1;
    return false;
}

QSqlQuery DatabaseManager::getPermissions(const QString& role)
{
    QSqlQuery q(m_db);
    q.prepare("SELECT module_name, can_view, can_create, can_edit, can_delete FROM RolePermissions WHERE role_name = :role ORDER BY module_name");
    q.bindValue(":role", role);
    q.exec();
    return q;
}

void DatabaseManager::seedDefaultAdmin()
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM Users WHERE username = 'admin'");
    query.exec();

    if (query.next() && query.value(0).toInt() == 0) {
        // Default password: admin123 — In production, use proper hashing
        query.prepare(
            "INSERT INTO Users (username, password_hash, full_name, role) "
            "VALUES ('admin', 'admin123', 'Administrator', 'admin')"
        );
        query.exec();
        qInfo() << "Default admin user created (admin / admin123)";
    }

    // Seed default settings
    QMap<QString, QString> defaults = {
        {"company_name",     "Security Guard Management Agency"},
        {"company_logo",     ""},
        {"pf_rate",          "12"},
        {"esic_rate",        "0.75"},
        {"pt_deduction",     "200"},
        {"overtime_rate",    "1.5"},
        {"casual_leave",     "12"},
        {"sick_leave",       "7"},
        {"earned_leave",     "15"},
        {"auto_backup",      "0"},
        {"backup_interval",  "7"}
    };

    for (auto it = defaults.begin(); it != defaults.end(); ++it) {
        query.prepare("INSERT OR IGNORE INTO Settings (key, value) VALUES (:k, :v)");
        query.bindValue(":k", it.key());
        query.bindValue(":v", it.value());
        query.exec();
    }
}

// ═══════════════════════════════════════════════════════════════
//  AUTHENTICATION
// ═══════════════════════════════════════════════════════════════

bool DatabaseManager::validateUser(const QString& username, const QString& password,
                                    QString& role, int& userId)
{
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, role FROM Users "
        "WHERE username = :user AND password_hash = :pass AND is_active = 1"
    );
    query.bindValue(":user", username);
    query.bindValue(":pass", password);
    query.exec();

    if (query.next()) {
        userId = query.value("id").toInt();
        role = query.value("role").toString();
        return true;
    }
    return false;
}

// ═══════════════════════════════════════════════════════════════
//  GUARDS
// ═══════════════════════════════════════════════════════════════

QVariantList DatabaseManager::getAllGuards()
{
    QVariantList list;
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT g.*, c.company_name AS client_name, s.site_name "
        "FROM Guards g "
        "LEFT JOIN Clients c ON g.client_id = c.id "
        "LEFT JOIN Sites s ON g.site_id = s.id "
        "ORDER BY g.full_name"
    );
    query.exec();

    while (query.next()) {
        QVariantMap row;
        QSqlRecord rec = query.record();
        for (int i = 0; i < rec.count(); ++i) {
            row[rec.fieldName(i)] = query.value(i);
        }
        list.append(row);
    }
    return list;
}

QVariantMap DatabaseManager::getGuardById(int id)
{
    QVariantMap data;
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT g.*, c.company_name AS client_name, s.site_name "
        "FROM Guards g "
        "LEFT JOIN Clients c ON g.client_id = c.id "
        "LEFT JOIN Sites s ON g.site_id = s.id "
        "WHERE g.id = :id"
    );
    query.bindValue(":id", id);
    query.exec();

    if (query.next()) {
        QSqlRecord rec = query.record();
        for (int i = 0; i < rec.count(); ++i) {
            data[rec.fieldName(i)] = query.value(i);
        }
    }
    return data;
}

bool DatabaseManager::insertGuard(const QVariantMap& data)
{
    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO Guards (
            guard_code, full_name, father_name, date_of_birth, gender,
            marital_status, mobile_primary, mobile_secondary, email,
            address_line1, address_line2, city, state, pincode,
            aadhaar_number, pan_number, photo_path,
            police_verified, police_verify_date,
            joining_date, status,
            bank_name, bank_account, bank_ifsc,
            uan_number, esic_number, pf_number,
            basic_salary, hra, conveyance, medical_allowance, special_allowance,
            site_id, client_id, notes
        ) VALUES (
            :guard_code, :full_name, :father_name, :date_of_birth, :gender,
            :marital_status, :mobile_primary, :mobile_secondary, :email,
            :address_line1, :address_line2, :city, :state, :pincode,
            :aadhaar_number, :pan_number, :photo_path,
            :police_verified, :police_verify_date,
            :joining_date, :status,
            :bank_name, :bank_account, :bank_ifsc,
            :uan_number, :esic_number, :pf_number,
            :basic_salary, :hra, :conveyance, :medical_allowance, :special_allowance,
            :site_id, :client_id, :notes
        )
    )");

    for (auto it = data.begin(); it != data.end(); ++it) {
        query.bindValue(":" + it.key(), it.value());
    }

    return query.exec();
}

bool DatabaseManager::updateGuard(int id, const QVariantMap& data)
{
    QStringList setClauses;
    for (auto it = data.begin(); it != data.end(); ++it) {
        setClauses << it.key() + " = :" + it.key();
    }
    setClauses << "updated_at = datetime('now','localtime')";

    QString sql = "UPDATE Guards SET " + setClauses.join(", ") + " WHERE id = :id";

    QSqlQuery query(m_db);
    query.prepare(sql);
    for (auto it = data.begin(); it != data.end(); ++it) {
        query.bindValue(":" + it.key(), it.value());
    }
    query.bindValue(":id", id);

    return query.exec();
}

bool DatabaseManager::deleteGuard(int id)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM Guards WHERE id = :id");
    query.bindValue(":id", id);
    return query.exec();
}

int DatabaseManager::getGuardCount()
{
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM Guards");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::getActiveGuardCount()
{
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM Guards WHERE status = 'Active'");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::getPresentTodayCount()
{
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT COUNT(*) FROM Attendance "
        "WHERE date = date('now','localtime') AND status = 'Present'"
    );
    query.exec();
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::getAbsentTodayCount()
{
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT COUNT(*) FROM Attendance "
        "WHERE date = date('now','localtime') AND status = 'Absent'"
    );
    query.exec();
    return query.next() ? query.value(0).toInt() : 0;
}

// ═══════════════════════════════════════════════════════════════
//  DASHBOARD STATS
// ═══════════════════════════════════════════════════════════════

int DatabaseManager::getClientCount()
{
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM Clients");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::getSiteCount()
{
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM Sites");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::getPendingSalaryCount()
{
    QSqlQuery query(m_db);
    query.exec("SELECT COUNT(*) FROM Salary WHERE payment_status = 'Pending'");
    return query.next() ? query.value(0).toInt() : 0;
}

int DatabaseManager::getTodayIncidentCount()
{
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT COUNT(*) FROM Incidents WHERE date(date_time) = date('now','localtime')"
    );
    query.exec();
    return query.next() ? query.value(0).toInt() : 0;
}
