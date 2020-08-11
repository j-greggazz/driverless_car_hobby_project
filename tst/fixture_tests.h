#ifndef FIXTURE_TESTS_H
#define FIXTURE_TESTS_H

#pragma once

#include <gtest/gtest.h>
#include <iostream>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <direct.h>
#include <dashboardTracker.h>
#include <QApplication>
#include <QWidget>
#include <mainwindow.h>





/*
#include <bank_account.h>

class BankAccountTest : public testing::Test {

public:
	BankAccount* m_account;

	BankAccountTest() : m_account(new BankAccount) {};

	~BankAccountTest() {
		delete m_account;
	}

	struct account_state {
		int initial_balance;
		int withdraw_amount;
		int final_balance;
		bool success;
	};

};

class ParamTest_withdraw : public BankAccountTest, public testing::WithParamInterface<BankAccountTest::account_state> {

public:
	ParamTest_withdraw() {
		m_account->m_balance = GetParam().initial_balance;

	}

};
*/
#endif