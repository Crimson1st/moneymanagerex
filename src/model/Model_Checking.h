/*******************************************************
 Copyright (C) 2013,2014 Guan Lisheng (guanlisheng@gmail.com)

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ********************************************************/

#ifndef MODEL_CHECKING_H
#define MODEL_CHECKING_H

#include "Model.h"
#include "db/DB_Table_Checkingaccount_V1.h"
#include "Model_Splittransaction.h"
#include "Model_CustomField.h"

class Model_Checking : public Model<DB_Table_CHECKINGACCOUNT_V1>
{
public:
    using Model<DB_Table_CHECKINGACCOUNT_V1>::remove;
    using Model<DB_Table_CHECKINGACCOUNT_V1>::save;
    typedef Model_Splittransaction::Data_Set Split_Data_Set;

public:
    enum TYPE { WITHDRAWAL = 0, DEPOSIT, TRANSFER };
    enum STATUS_ENUM { NONE = 0, RECONCILED, VOID_, FOLLOWUP, DUPLICATE_};

    static const std::vector<std::pair<TYPE, wxString> > TYPE_CHOICES;
    static const std::vector<std::pair<STATUS_ENUM, wxString> > STATUS_ENUM_CHOICES;

public:
    struct Full_Data: public Data
    {
        Full_Data();
        explicit Full_Data(const Data& r);
        Full_Data(const Data& r
            , const std::map<int /*trans id*/
                , Model_Splittransaction::Data_Set /*split trans*/ > & splits);

        ~Full_Data();
        wxString ACCOUNTNAME, TOACCOUNTNAME;
        wxString PAYEENAME;
        wxString CATEGNAME;
        wxString displayID;
        double AMOUNT;
        double BALANCE;
        wxArrayString ATTACHMENT_DESCRIPTION;
        Model_Splittransaction::Data_Set m_splits;
        wxString real_payee_name(int account_id) const;
        const wxString get_currency_code(int account_id) const;
        const wxString get_account_name(int account_id) const;
        bool has_split() const;
        bool has_attachment() const;
        bool is_foreign() const;
        bool is_foreign_transfer() const;

        wxString info() const;
        const wxString to_json();

        // Reserved string variables for custom data
        wxString UDFC01;
        double UDFC01_val;
        Model_CustomField::FIELDTYPE UDFC01_Type;
        wxString UDFC02;
        double UDFC02_val;
        Model_CustomField::FIELDTYPE UDFC02_Type;
        wxString UDFC03;
        double UDFC03_val;
        Model_CustomField::FIELDTYPE UDFC03_Type;
        wxString UDFC04;
        double UDFC04_val;
        Model_CustomField::FIELDTYPE UDFC04_Type;
        wxString UDFC05;
        double UDFC05_val;
        Model_CustomField::FIELDTYPE UDFC05_Type;
    };
    typedef std::vector<Full_Data> Full_Data_Set;

    struct SorterByBALANCE
    { 
        template<class DATA>
        bool operator()(const DATA& x, const DATA& y)
        {
            return x.BALANCE < y.BALANCE;
        }
    };
    struct SorterByDEPOSIT
    {
        template<class DATA>
        bool operator()(const DATA& x, const DATA& y)
        {
            return x.AMOUNT < y.AMOUNT;
        }
    };
    struct SorterByWITHDRAWAL
    {
        template<class DATA>
        bool operator()(const DATA& x, const DATA& y)
        {
            return x.AMOUNT > y.AMOUNT;
        }
    };
    struct SorterByNUMBER
    {
        template<class DATA>
        bool operator()(const DATA& x, const DATA& y)
        {
            return x.TRANSACTIONNUMBER.IsNumber() && y.TRANSACTIONNUMBER.IsNumber()
                ? (wxAtoi(x.TRANSACTIONNUMBER) < wxAtoi(y.TRANSACTIONNUMBER))
                : x.TRANSACTIONNUMBER < y.TRANSACTIONNUMBER;
        }
    };
public:
    Model_Checking();
    ~Model_Checking();

public:
    static wxArrayString all_type();
    static wxArrayString all_status();
    static const wxString TRANSFER_STR;
    static const wxString WITHDRAWAL_STR;
    static const wxString DEPOSIT_STR;

public:
    /**
    Initialize the global Model_Checking table on initial call.
    Resets the global table on subsequent calls.
    * Return the static instance address for Model_Checking table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static Model_Checking& instance(wxSQLite3Database* db);

    /**
    * Return the static instance address for Model_Checking table
    * Note: Assigning the address to a local variable can destroy the instance.
    */
    static Model_Checking& instance();

public:
    bool remove(int id);
    int save(Data* r);
    int save(std::vector<Data>& rows);
    int save(std::vector<Data*>& rows);
    void updateTimestamp(int id);
public:
    static const Model_Splittransaction::Data_Set splittransaction(const Data* r);
    static const Model_Splittransaction::Data_Set splittransaction(const Data& r);

public:
    static DB_Table_CHECKINGACCOUNT_V1::TRANSDATE TRANSDATE(const wxDate& date, OP op = EQUAL);
    static DB_Table_CHECKINGACCOUNT_V1::DELETEDTIME DELETEDTIME(const wxString& date, OP op = EQUAL);
    static DB_Table_CHECKINGACCOUNT_V1::TRANSDATE TRANSDATE(const wxString& date_iso_str, OP op = EQUAL);
    static DB_Table_CHECKINGACCOUNT_V1::STATUS STATUS(STATUS_ENUM status, OP op = EQUAL);
    static DB_Table_CHECKINGACCOUNT_V1::TRANSCODE TRANSCODE(TYPE type, OP op = EQUAL);
    static DB_Table_CHECKINGACCOUNT_V1::TRANSACTIONNUMBER TRANSACTIONNUMBER(const wxString& num, OP op = EQUAL);

public:
    static wxDate TRANSDATE(const Data* r);
    static wxDate TRANSDATE(const Data& r);
    static TYPE type(const wxString& r);
    static TYPE type(const Data* r);
    static TYPE type(const Data& r);
    static STATUS_ENUM status(const wxString& r);
    static STATUS_ENUM status(const Data* r);
    static STATUS_ENUM status(const Data& r);
    static double amount(const Data* r, int account_id = -1);
    static double amount(const Data&r, int account_id = -1);
    static double balance(const Data* r, int account_id = -1);
    static double balance(const Data& r, int account_id = -1);
    static double withdrawal(const Data* r, int account_id = -1);
    static double withdrawal(const Data& r, int account_id);
    static double deposit(const Data* r, int account_id);
    static double deposit(const Data& r, int account_id);
    static double reconciled(const Data* r, int account_id);
    static double reconciled(const Data& r, int account_id);
    static bool is_locked(const Data* r);
    static bool is_transfer(const wxString& r);
    static bool is_transfer(const Data* r);
    static bool is_deposit(const wxString& r);
    static bool is_deposit(const Data* r);
    static wxString toShortStatus(const wxString& fullStatus);
    static void getFrequentUsedNotes(std::vector<wxString> &frequentNotes, int accountID = -1);
    static void getEmptyTransaction(Data &data, int accountID);
    static bool getTransactionData(Data &data, const Data* r);
    static void putDataToTransaction(Data *r, const Data &data);
    static bool foreignTransaction(const Data& data);
    static bool foreignTransactionAsTransfer(const Data& data);
};

inline bool Model_Checking::Full_Data::has_split() const { return !this->m_splits.empty(); }
inline bool Model_Checking::Full_Data::has_attachment() const { return !ATTACHMENT_DESCRIPTION.empty(); }

#endif // 
