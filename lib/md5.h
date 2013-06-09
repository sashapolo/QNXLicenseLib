//! Заголовочный файл с прототипами функций для подсчета хэша md5.
/*!
 * @file
 * @author  Александр Половцев (sasha_polo@mail.ru)
 * @date    25.04.2013
 *
 * Реализация взята из RFC 1321 с небольшими модификациями, чтобы
 * соответствовать общему стилю библиотеки.
 *
 * @see http://www.ietf.org/rfc/rfc1321.txt
 */


/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
   rights reserved.

   License to copy and use this software is granted provided that it
   is identified as the "RSA Data Security, Inc. MD5 Message-Digest
   Algorithm" in all material mentioning or referencing this software
   or this function.

   License is also granted to make and use derivative works provided
   that such works are identified as "derived from the RSA Data
   Security, Inc. MD5 Message-Digest Algorithm" in all material
   mentioning or referencing the derived work.

   RSA Data Security, Inc. makes no representations concerning either
   the merchantability of this software or the suitability of this
   software for any particular purpose. It is provided "as is"
   without express or implied warranty of any kind.

   These notices must be retained in any copies of any part of this
   documentation and/or software.
 */

#ifndef MD5_H_
#define MD5_H_

//! 32-битный целочисленный тип.
typedef unsigned int uint4;

//! Вспомогательный контекст для подсчета хэша md5.
typedef struct {
    uint4 state[4];           /*!< 4 переменных для хранения промежуточных результатов, отображающих состояние (ABCD). */
    uint4 count[2];           /*!< Счетчик количества бит. */
    unsigned char buffer[64]; /*!< Буфер с данными. */
} md5_context_t;

//! Инициализация контекста
/*!
 * Функция инициализирует переданный ей контекст начальными значениями.
 * Ее необходимо вызывать перед каждой операцией получения хэша.
 *
 * @param [out] context инициализируемый контекст
 */
void md5_init(md5_context_t* context);
//! Обновление контекста.
/*!
 * Функция обрабатывает новый блок сообщения и обновляет переданный ей контекст.
 *
 * @param [out] context инициализируемый контекст
 * @param [in] msg блок сообщения
 * @param [in] len размер блока
 */
void md5_update(md5_context_t* context, const unsigned char* msg, size_t len);
//! Завершение операции получения хэша.
/*!
 * Функция записывает значение хэша в @p msg и обнуляет контекст.
 *
 * @param [out] context инициализируемый контекст
 * @param [out] msg хэш
 */
void md5_finalize(md5_context_t* context, unsigned char* msg);

#endif /* MD5_H_ */
